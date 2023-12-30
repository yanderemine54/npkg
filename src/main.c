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

#include <archive.h>
#include <curl/curl.h>
#include <gpgme.h>
#include <toml.h>

#include "extract.h"
#include "download.h"
#include "verify.h"
#include "find.h"

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        puts("Usage: npkg package-to-install");
        return EXIT_FAILURE;
    }
    CURL* handle;
    struct FtpFile package_list = {
        "packages.list",
        NULL
    };
    FILE* configFile = fopen("/etc/npkg.conf", "r");
    char* tomlErrorBuffer = calloc(512, sizeof(char));
    char* repository = calloc(497, sizeof(char));
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

    /* Dowload package list & packages */
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, strncat(url, "/packages.list", 15));
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writefunction);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &package_list);
    curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_perform(handle);
    fclose(package_list.stream);
    const char* packageToInstall = argv[1];

    char* packageName = findPackageInRepository(packageToInstall);
    puts("Downloading packages...");
    download_package(handle, packageName, repository);
    curl_easy_cleanup(handle);
    curl_global_cleanup();

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
    strncat(packageFilename, "hello-2.12.1", 504);
    strncat(packageFilename, ".tar.zst", 8);
    strncat(signatureFilename, packageFilename, 512);
    strncat(signatureFilename, ".sig", 5);
    if (verify_package(context, signatureFilename, packageFilename) == VERIFY_OK) {
        puts("Extracting packages...");
        extract_package(packageFilename);
        char* packagePath = calloc(512, sizeof(char));
        strncat(packagePath, "./", 3);
        strncat(packagePath, packageName, 509);
        chdir(packagePath);
        system("./configure");
        printf("Source configured.\n");
        system("make");
        printf("Source compiled.\n");
        if (geteuid() == 0) {
            system("make install");
            printf("Package installed.\n");
        }
        free(packagePath);
    } else {
        fprintf(stderr, "ERROR: package %s corrupt or tampered with!\n", packageFilename);
    }
    free(packageFilename);
    free(signatureFilename);
    gpgme_release(context);

    free(repository);
    toml_free(config);
    return EXIT_SUCCESS;
}