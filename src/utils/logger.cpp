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

#include <cassert>
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

logger::Logger* logger::Logger::Instance() {
    static Logger logger;
    return &logger;
}

void logger::Logger::Log(const logger::Level level, const char* file,
                         const int line, const char* function,
                         const char* format, ...) {
    va_list args;
    va_start(args, format);
    this->Vlog(level, file, line, function, format, args);
    va_end(args);
}

int logger::Logger::SetLevel(const logger::Level level) {
#ifndef NDEBUG
    const int max = logger::LEVEL_DEBUG;
#else
    const int max = logger::LEVEL_INFO;
#endif
    if (level > max) return 1;
    minLevel = level;
    return 0;
}

logger::Level logger::Logger::GetLevel() const { return minLevel; }

const char* logger::Logger::Level2String(const Level level) {
    switch (level) {
        case LEVEL_ERROR:
            return "ERROR";
        case LEVEL_WARNING:
            return "WARNING";
        case LEVEL_INFO:
            return "INFO";
#ifndef NDEBUG
        case LEVEL_DEBUG:
            return "DEBUG";
#endif
        default:
            assert(0 && "unreachable");
    }
}

void logger::Logger::Vlog(const Level level, const char* file, const int line,
                          const char* function, const char* format,
                          va_list args) {
    // Filter out messages below the configured minimum level. Levels are
    // ordered from most severe (LEVEL_ERROR = 0) to most verbose
    // (LEVEL_DEBUG = 3). If the message level is greater (less severe) than
    // the configured minimum, do not print it.
    if (level > this->minLevel) {
        return;
    }

    FILE* output =
        (level == LEVEL_ERROR || level == LEVEL_WARNING) ? stderr : stdout;

    // Print only the basename of the source file (not the full path).
#ifdef _WIN32
    const char* base = file;
    const char* lastBack = strrchr(file, '\\');
    if (lastBack) base = lastBack + 1;
#else
    const char* base = file;
    const char* lastSlash = strrchr(file, '/');
    if (lastSlash) base = lastSlash + 1;
#endif

    // Add colors for different levels when output is a terminal. Use ANSI
    // escape sequences and only enable them if the output file descriptor is a
    // tty. Use a separate color for the file/function names.
    const char* levelColor = "";
    const char* nameColor = "";
    const char* colorEnd = "";
    int fd = FILENO(output);
    if (ISATTY(fd)) {
        switch (level) {
            case LEVEL_ERROR:
                levelColor = "\x1b[31m";  // Red.
                nameColor = "\x1b[35m";   // Magenta for file/function.
                break;
            case LEVEL_WARNING:
                levelColor = "\x1b[33m";  // Yellow.
                nameColor = "\x1b[35m";   // Magenta for file/function.
                break;
            case LEVEL_INFO:
                levelColor = "\x1b[32m";  // Green.
                nameColor = "\x1b[34m";   // Blue for file/function.
                break;
#ifndef NDEBUG
            case LEVEL_DEBUG:
                levelColor = "\x1b[36m";  // Cyan.
                nameColor = "\x1b[34m";   // Blue for file/function.
                break;
#endif
            default:
                assert(0 && "Unreachable");
        }
        colorEnd = "\x1b[0m";
    }

    /* Print level with its color, then the file in the terminal's default
     * color, and the function name in the distinct name color. */
    fprintf(output, "%s[%s]%s[%s:%d]%s[%s]%s ", levelColor, Level2String(level),
            colorEnd, base, line, nameColor, function, colorEnd);
    vfprintf(output, format, args);
}
