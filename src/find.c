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

#include "find.h"

#define MIN(a, b) a < b ? a : b

char* findPackageInRepository(const char* name) {
    char* packageString = calloc(256, sizeof(char));
    FILE* packageList = fopen("packages.list", "r");
    char* lineBuffer = calloc(256, sizeof(char));
    char* token = calloc(256, sizeof(char));
    while (!feof(packageList)) {
        fgets(lineBuffer, 256, packageList);
        token = strtok(lineBuffer, " ");
        if (strncmp(token, name, MIN(strlen(token), strlen(name))) == 0) {
            strncat(packageString, name, MIN(strlen(name), 512));
            strncat(packageString, "-", 2);
            token = strtok(NULL, " ");
            strncat(packageString, token, MIN(256 - strlen(name), strlen(token) - 1));
            break;
        }
    }
    free(lineBuffer);
    fclose(packageList);
    return packageString;
}
