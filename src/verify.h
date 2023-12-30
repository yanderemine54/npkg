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

#ifndef NPKG_VERFIY_H
#define NPKG_VERIFY_H
#define VERIFY_OK 0
#define VERIFY_ERR 1
int verify_package(gpgme_ctx_t context, const char* signatureFilename, const char* packageFilename);
#endif
