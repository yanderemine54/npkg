// Copyright (C) 2024 Yanderemine54
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

#ifndef NPKG_COLORS_H
#define NPKG_COLORS_H

/*** ANSI X3.64 Basic control sequences. ***/

#define ANSI_RESET             "\033[0m"
#define ANSI_BOLD              "\033[1m"
#define ANSI_FAINT             "\033[2m"
#define ANSI_ITALIC            "\033[3m"
#define ANSI_UNDERLINE         "\033[4m"
#define ANSI_SLOW_BLINK        "\033[5m"
#define ANSI_FAST_BLINK        "\033[6m"
#define ANSI_REVERSE           "\033[7m"
#define ANSI_HIDE              "\033[8m"
#define ANSI_STRIKETHROUGH     "\033[9m"

/***       ANSI X3.64 4-bit color.       ***/

/**              Foreground.              **/

#define ANSI_FG_BLACK          "\033[30m"
#define ANSI_FG_RED            "\033[31m"
#define ANSI_FG_GREEN          "\033[32m"
#define ANSI_FG_YELLOW         "\033[33m"
#define ANSI_FG_BLUE           "\033[34m"
#define ANSI_FG_MAGENTA        "\033[35m"
#define ANSI_FG_CYAN           "\033[36m"
#define ANSI_FG_WHITE          "\033[37m"
#define ANSI_FG_BRIGHT_BLACK   "\033[90m"
#define ANSI_FG_BRIGHT_RED     "\033[91m"
#define ANSI_FG_BRIGHT_GREEN   "\033[92m"
#define ANSI_FG_BRIGHT_YELLOW  "\033[93m"
#define ANSI_FG_BRIGHT_BLUE    "\033[94m"
#define ANSI_FG_BRIGHT_MAGENTA "\033[95m"
#define ANSI_FG_BRIGHT_CYAN    "\033[96m"
#define ANSI_FG_BRIGHT_WHITE   "\033[97m"

/**              Background.              **/

#define ANSI_BG_BLACK          "\033[40m"
#define ANSI_BG_RED            "\033[41m"
#define ANSI_BG_GREEN          "\033[42m"
#define ANSI_BG_YELLOW         "\033[43m"
#define ANSI_BG_BLUE           "\033[44m"
#define ANSI_BG_MAGENTA        "\033[45m"
#define ANSI_BG_CYAN           "\033[46m"
#define ANSI_BG_WHITE          "\033[47m"
#define ANSI_BG_BRIGHT_BLACK   "\033[100m"
#define ANSI_BG_BRIGHT_RED     "\033[101m"
#define ANSI_BG_BRIGHT_GREEN   "\033[102m"
#define ANSI_BG_BRIGHT_YELLOW  "\033[103m"
#define ANSI_BG_BRIGHT_BLUE    "\033[104m"
#define ANSI_BG_BRIGHT_MAGENTA "\033[105m"
#define ANSI_BG_BRIGHT_CYAN    "\033[106m"
#define ANSI_BG_BRIGHT_WHITE   "\033[107m"
#endif