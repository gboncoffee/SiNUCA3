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
    fprintf(output, "[%s][%s:%d][%s] ", level_to_string(level), file, line,
            function);
    vfprintf(output, format, args);
}
