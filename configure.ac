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

AC_INIT([npkg],[0.3.0],[maddy@maddysworld.de],[npkg],[https://github.com/yanderemine54/npkg])
AC_PREREQ([2.71])
AM_INIT_AUTOMAKE([foreign])
AC_USE_SYSTEM_EXTENSIONS
gl_INIT
AC_CONFIG_MACRO_DIRS([m4])

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
gl_EARLY

# Header shenanigans for debug messages.
AH_TEMPLATE([DEBUG], [Define to enable debug messages.])
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug],[enable debug messages (default is NO)]), [
    case "${enableval}" in
    yes) npkg_opt_debug=yes ;;
    no) npkg_opt_debug=no ;;
    *) npkg_opt_debug=no ;;
    esac
], [npkg_opt_debug=no])
test "$npkg_opt_debug" = "yes" && AC_DEFINE([DEBUG])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile])
AC_OUTPUT
