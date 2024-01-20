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
#include <sys/stat.h>

#include <curl/curl.h>

#include "download.h"
#include "config.h"

size_t writefunction(void* buffer, size_t size, size_t nmemb, void* stream) {
    struct FtpFile* out = (struct FtpFile* )stream;
    if (!out->stream) {
        out->stream = fopen(out->filename, "wb");
        if (!out->stream)
            return 0;
    }

    return fwrite(buffer, size, nmemb, out->stream);
}

int download_package(CURL* handle, const char* package, const char* repository) {
    struct stat source_temp_dir_stat;
    char* url = (char*)calloc(1024, sizeof(char));
    char* packageFilename = (char*)calloc(512, sizeof(char));
    strcat(packageFilename, "/tmp/npkg/src/");
    strncat(packageFilename, package, 503);
    strncat(packageFilename, ".tar.zst", 9);
    char* packageSignatureFilename = (char*)calloc(512, sizeof(char));
    strncpy(packageSignatureFilename, packageFilename, 507);
    strncat(packageSignatureFilename, ".sig", 5);
    char* remotePackageFilename = (char*)calloc(512, sizeof(char));
    strncat(remotePackageFilename, package, 503);
    strncat(remotePackageFilename, ".tar.zst", 9);
    struct FtpFile packageArchive = {
        packageFilename,
        NULL
    };
    struct FtpFile packageSignature = {
        packageSignatureFilename,
        NULL
    };
#ifdef DEBUG
    puts(packageFilename);
    puts(packageSignatureFilename);
#endif
    /* Create the temporary source directory if it doesn't exist. */
    if (stat("/tmp/npkg/src", &source_temp_dir_stat) == -1) {
        mkdir("/tmp/npkg", 0755);
        mkdir("/tmp/npkg/src", 0755);
    }

    /* Build final URL */
    strncat(url, repository, 506);
    strncat(url, "/", 2);
    strncat(url, remotePackageFilename, 512);
#ifdef DEBUG
    puts(url);
#endif
    /* Download package */
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writefunction);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &packageArchive);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    CURLcode error = curl_easy_perform(handle);
#ifdef DEBUG
    printf("%s\n", curl_easy_strerror(error));
#endif
    /* Build url of signature */
    strncat(url, ".sig", 5);
#ifdef DEBUG
    puts(url);
#endif
    /* Download the package's signature */
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writefunction);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &packageSignature);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    error = curl_easy_perform(handle);
#ifdef DEBUG
    printf("%s\n", curl_easy_strerror(error));
#endif
    fclose(packageArchive.stream);
    fclose(packageSignature.stream);
    free(url);
    free(packageFilename);
    free(packageSignatureFilename);
    free(remotePackageFilename);
    return 0;
}