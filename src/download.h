// Copyright (C) 2023 Yanderemine54
// 
// This file is part of npkg.
// 
// npkg is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// npkg is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with npkg.  If not, see <http://www.gnu.org/licenses/>.

#ifndef NPKG_DOWNLOAD_H
#define NPKG_DOWNLOAD_H
struct FtpFile {
    const char* filename;
    FILE* stream;
};

size_t writefunction(void* buffer, size_t size, size_t nmemb, void* stream);
int download_package(CURL* handle, const char* package, const char* repository);
#endif