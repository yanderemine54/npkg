/**
 * Copyright (C) 2023 Yanderemine54
 * 
 * This file is part of npkg.
 * 
 * npkg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * npkg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with npkg.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include <archive.h>
#include "argp.h" /* In source tree for portability reasons. */
#include <curl/curl.h>
#include <gpgme.h>
#include <toml.h>

#include "extract.h"
#include "download.h"
#include "verify.h"
#include "find.h"
#include "colors.h"

/* Argp boilerplate. */
const char* argp_program_version = "npkg 0.3.0";
const char* argp_program_bug_address = "<maddy@maddysworld.de>";
static char doc[] = "A source-based package manager written in C.";
static char args_doc[] = "[PACKAGE-NAME]...";
static struct argp_option options[] = {
    { "install", 'i', 0, OPTION_ARG_OPTIONAL,                                 "Install a package." },
    { "update",  'u', 0, OPTION_ARG_OPTIONAL,                                  "Update a package." },
    { "search",  's', 0, OPTION_ARG_OPTIONAL, "Search for a package in the configured repository." },
    { "delete",  'd', 0, OPTION_ARG_OPTIONAL,                               "Uninstall a package." },
    { 0 }
};

struct arguments {
    enum {
        INSTALL_MODE,
        UPDATE_MODE,
        SEARCH_MODE,
        DELETE_MODE
    } mode;
    char* package;
};

static error_t parse_options(int key, char* argument, struct argp_state* state) {
    struct arguments* arguments = state->input;
    switch (key) {
        case 'i':
            arguments->mode = INSTALL_MODE;
            break;
        case 'u':
            arguments->mode = UPDATE_MODE;
            break;
        case 's':
            arguments->mode = SEARCH_MODE;
            break;
        case 'd':
            arguments->mode = DELETE_MODE;
            break;
        case ARGP_KEY_ARG:
            arguments->package = argument;
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp parser = {
    options,
    parse_options,
    args_doc,
    doc,
    0,
    0,
    0
};

void recursivelyRemoveFileTree(const char* path);

int main(int argc, const char* argv[]) {
    CURL* handle;
    struct FtpFile package_list = {
        "packages.list",
        NULL
    };
    struct arguments arguments;
    arguments.mode = INSTALL_MODE;
    arguments.package = calloc(256, sizeof(char));

    argp_parse(&parser, argc, (char**)argv, 0, 0, &arguments);

    FILE* configFile = fopen("/etc/npkg.conf", "r");
    char* repository = calloc(497, sizeof(char));
    if (!configFile) {
        repository = "ftp://anonymous@ftp.maddysworld.de/packages";
        goto config_set;
    }
    char* tomlErrorBuffer = calloc(512, sizeof(char));
    toml_table_t* config = toml_parse_file(configFile, tomlErrorBuffer, 512);
    fclose(configFile);
    if (!config) {
        printf("Couldn\'t parse config: %s\nUsing default config.\n", tomlErrorBuffer);
        repository = "ftp://anonymous@ftp.maddysworld.de/packages";
        goto config_set;
    }
    toml_table_t* systemEnvrionmentTable = toml_table_in(config, "system");
    if (!systemEnvrionmentTable) {
        printf("Missing [system].\nUsing default config.\n");
        repository = "ftp://anonymous@ftp.maddysworld.de/packages";
        goto config_set;
    }
    toml_array_t* repositoryArray = toml_array_in(systemEnvrionmentTable, "repositories");
    if (!repositoryArray) {
        printf("Missing `repositories` array.\nUsing default config.");
        repository = "ftp://anonymous@ftp.maddysworld.de/packages";
        goto config_set;
    }
    toml_datum_t repositoryString = toml_string_at(repositoryArray, 0);
    if (!repositoryString.ok) {
        printf("The `repository` array is empty.\nUsing default config.\n");
        repository = "ftp://anonymous@ftp.maddysworld.de/packages";
        goto config_set;
    }
    repository = repositoryString.u.s;
config_set:
    char* url = (char*)calloc(512, sizeof(char));
    strncat(url, repository, 497);

    if (arguments.mode == INSTALL_MODE || arguments.mode == SEARCH_MODE || arguments.mode == UPDATE_MODE) {
        int error = 0;
        /* Download package list. */
        curl_global_init(CURL_GLOBAL_ALL);
        handle = curl_easy_init();
        curl_easy_setopt(handle, CURLOPT_URL, strncat(url, "/packages.list", 15));
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writefunction);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &package_list);
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_perform(handle);
        fclose(package_list.stream);
        char* packageName = findPackageInRepository(arguments.package);
        unlink("packages.list");

        if (arguments.mode == INSTALL_MODE || arguments.mode == UPDATE_MODE) {
            /* We're installing or updating a package. */
            struct stat directory;
            char* package_path = calloc(256, sizeof(char));
            strcat(package_path, "/usr/local/");
            strncat(package_path, packageName, 255-strlen( "/usr/local/"));
            if (arguments.mode == UPDATE_MODE && stat(package_path, &directory) == 0 && S_ISDIR(directory.st_mode)) {
                /* We're already on the latest version. */
                printf("%s is already the latest version available via the repository, no update required :)\n", arguments.package);
                free(package_path);
                goto cleanup;
            }
            free(package_path);
            if (arguments.mode == UPDATE_MODE) {
                /* TODO: Keep track of installed files in install script and 
                   delete those instead of semi-hardcoding the path. */
                char* binaryPath = calloc(256, sizeof(char));
                strcat(binaryPath, "/usr/local/bin/");
                strncat(binaryPath, arguments.package, 255-strlen("/usr/local/bin/"));
                unlink(binaryPath);
                free(binaryPath);
            }
            /* Download packages. */
            puts("Downloading packages...");
            download_package(handle, packageName, repository);

            /* Verify package signature to prevent tampering and/or corruption of packages */
            puts("Verifying packages...");
            setlocale(LC_ALL, "");
            gpgme_check_version(NULL); /* Initialize GPGME (This library's naming onventions are weird.) */
            gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
            gpgme_engine_check_version(GPGME_PROTOCOL_OPENPGP);
            gpgme_ctx_t context;
            gpgme_new(&context);
            char* packageFilename = calloc(512, sizeof(char));
            char* signatureFilename = calloc(517, sizeof(char));
            strcpy(packageFilename, "/tmp/npkg/src/");
            strncat(packageFilename, packageName, 504);
            strncat(packageFilename, ".tar.zst", 8);
            strncpy(signatureFilename, packageFilename, 512);
            strncat(signatureFilename, ".sig", 5);
            error = verify_package(context, signatureFilename, packageFilename);
            if (error == VERIFY_ERR) {
                fprintf(stderr, "%sERROR%s: package %s corrupt or tampered with!\n", ANSI_FG_RED, ANSI_RESET, packageFilename);
                goto verify_cleanup;
            }

            puts("Extracting packages...");
            extract_package(packageFilename);
            char* packagePath = calloc(512, sizeof(char));
            strncat(packagePath, "./", 3);
            strncat(packagePath, packageName, 509);
            chdir(packagePath);
            system("./npkg-install.sh");
            free(packagePath);

verify_cleanup:
            free(packageFilename);
            free(signatureFilename);
            gpgme_release(context);
        } else {
            /* We're searching for a package. */
            strtok(packageName, "-");
            char* packageVersion = strtok(NULL, "-");
            if (packageVersion == NULL) {
                printf("Package `%s` doesn't exist in repository `%s`.\n", arguments.package, repository);
            } else {
                printf("Package `%s` exists in version `%s`.\n", packageName, packageVersion);
            }
        }
cleanup:
        curl_easy_cleanup(handle);
        curl_global_cleanup();
        if (error) {
            return EXIT_FAILURE;
        }
    } else {
        /* We're uninstalling a package. */
        /* Check if we have effective root. */
        if (geteuid() != 0) {
            fprintf(stderr, "%sERROR%s: We do not have sufficient permissions to uninstall the package.\nPlease try again as superuser.\n", ANSI_FG_RED, ANSI_RESET);
            return EXIT_FAILURE;
        }
        printf("Are you sure that you want to uninstall %s? [y/N] ", arguments.package);
        int answer = getchar();
        if (answer != (int)'y') {
            printf("Not uninstalling %s.\n", arguments.package);
            return EXIT_SUCCESS;
        }
        /* TODO: Keep track of installed files in install script and 
            delete those instead of semi-hardcoding the path. */
        char* binaryPath = calloc(256, sizeof(char));
        strcat(binaryPath, "/usr/local/bin/");
        strncat(binaryPath, arguments.package, 255-strlen("/usr/local/bin/"));
        unlink(binaryPath);
        free(binaryPath);
        /* Download package list and find the package name. */
        curl_global_init(CURL_GLOBAL_ALL);
        handle = curl_easy_init();
        curl_easy_setopt(handle, CURLOPT_URL, strncat(url, "/packages.list", 15));
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writefunction);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &package_list);
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_perform(handle);
        fclose(package_list.stream);
        char* packageName = findPackageInRepository(arguments.package);
        unlink("packages.list");
        char* packagePath = calloc(1024, sizeof(char));
        strcat(packagePath, "/usr/local/");
        strncat(packagePath, packageName, 255-strlen("/usr/local/"));
        /* Remove the whole installed tree. This is in another function for readability reasons. */
        recursivelyRemoveFileTree(packagePath);
        free(packagePath);
        curl_easy_cleanup(handle);
        curl_global_cleanup();
    }
    
    return EXIT_SUCCESS;
}

void recursivelyRemoveFileTree(const char* path) {
    size_t path_length;
    char* full_path;
    DIR* directory;
    struct stat stat_path, stat_entry;
    struct dirent* entry;

    stat(path, &stat_path);

    if (S_ISDIR(stat_path.st_mode) == 0) {
        fprintf(stderr, "%sERROR%s: %s isn't a directory!", ANSI_FG_RED, ANSI_RESET, path);
        return;
    }

    directory = opendir(path);
    path_length = strlen(path);

    while ((entry = readdir(directory)) != NULL) {
        /* Skip "." and "..", since we'll either delete them anyway, or won't delete them else it'd break the whole system. */
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }

        full_path = calloc(path_length + 1 + strlen(entry->d_name) + 1, sizeof(char));
        strcpy(full_path, path); /* This is safe to do, since full_path is guaranteed to have at least 2 bytes more than path. */
        strcat(full_path, "/");
        strcat(full_path, entry->d_name); /* Also safe, since full_path has at least one byte more than entry->d_name. */

        stat(full_path, &stat_entry);

        /* Recursion ftw! */
        if (S_ISDIR(stat_entry.st_mode) != 0) {
            recursivelyRemoveFileTree(full_path);
            free(full_path);
            continue;
        }

        /* This is a file. */
        unlink(full_path);
        free(full_path);
    }
    
    /* Our work is done; remove the directory! */
    rmdir(path);
    closedir(directory);
}
