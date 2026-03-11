//
// Copyright (C) 2026  HiPES - Universidade Federal do Paraná
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

/**
 * @file logger.cpp
 * @details Macros related to logging (in our case, just printing stuff to the
 * stdout or stderr).
 */

#include "logger.hpp"

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#define ISATTY(fd) _isatty(fd)
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY(fd) isatty(fd)
#define FILENO fileno
#endif

logger::Logger* logger::Logger::instance() {
    static Logger logger;
    return &logger;
}

void logger::Logger::log(const Level level, const char* file, const int line,
                         const char* function, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(level, file, line, function, format, args);
    va_end(args);
}

const char* logger::Logger::level_to_string(const Level level) {
    switch (level) {
        case LEVEL_ERROR:
            return "ERROR";
        case LEVEL_WARNING:
            return "WARNING";
        case LEVEL_INFO:
            return "INFO";
        case LEVEL_DEBUG:
            return "DEBUG";
        default:
            assert(0 && "unreachable");
    }
}

void logger::Logger::vlog(const Level level, const char* file, const int line,
                          const char* function, const char* format,
                          va_list args) {
    FILE* output =
        (level == LEVEL_ERROR || level == LEVEL_WARNING) ? stderr : stdout;

    /* Print only the basename of the source file (not the full path). */
    const char* base = file;
    const char* last_slash = strrchr(file, '/');
    const char* last_back = strrchr(file, '\\');
    const char* last = NULL;
    if (last_slash && last_back) {
        last = (last_slash > last_back) ? last_slash : last_back;
    } else if (last_slash) {
        last = last_slash;
    } else if (last_back) {
        last = last_back;
    }
    if (last) base = last + 1;

    /*
     * Add colors for different levels when output is a terminal. Use ANSI
     * escape sequences and only enable them if the output file descriptor is a
     * tty. Use a separate color for the file/function names.
     */
    const char* level_color = "";
    const char* name_color = "";
    const char* color_end = "";
    int fd = FILENO(output);
    if (ISATTY(fd)) {
        switch (level) {
            case LEVEL_ERROR:
                level_color = "\x1b[31m"; /* red */
                name_color = "\x1b[35m";  /* magenta for file/function */
                break;
            case LEVEL_WARNING:
                level_color = "\x1b[33m"; /* yellow */
                name_color = "\x1b[35m";  /* magenta for file/function */
                break;
            case LEVEL_INFO:
                level_color = "\x1b[32m"; /* green */
                name_color = "\x1b[34m";  /* blue for file/function */
                break;
            case LEVEL_DEBUG:
                level_color = "\x1b[36m"; /* cyan */
                name_color = "\x1b[34m";  /* blue for file/function */
                break;
            default:
                level_color = "";
                name_color = "";
        }
        color_end = "\x1b[0m";
    }

    /* Print level with its color, then the file in the terminal's default
     * color, and the function name in the distinct name color. */
    fprintf(output, "%s[%s]%s[%s:%d]%s[%s]%s ", level_color,
            level_to_string(level), color_end, base, line, name_color, function,
            color_end);
    vfprintf(output, format, args);
}
