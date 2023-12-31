# Copyright (C) 2023 Yanderemine54
# 
# This file is part of npkg.
# 
# npkg is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# npkg is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with npkg.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([npkg], [0.1.1])
AC_PREREQ([2.69])
AM_INIT_AUTOMAKE([foreign])

LIBCURL_CHECK_CONFIG([yes], [8.0.0])
AC_SEARCH_LIBS([curl_global_init], [curl])
AC_SEARCH_LIBS([ZSTD_decompress], [zstd])
AC_SEARCH_LIBS([archive_read_new], [archive])
AM_PATH_GPGME([1.8.0], [
    AC_SEARCH_LIBS([gpg_strerror], [gpg-error])
])
AC_SEARCH_LIBS([toml_parse_file], [toml])

# enable large file support
AC_SYS_LARGEFILE

AC_PROG_CC

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
