#ifndef SINUCA3_UTILS_LOGGER_HPP_
#define SINUCA3_UTILS_LOGGER_HPP_

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
 * @file logger.hpp
 * @details Public API of the SiNUCA3 logging system.
 */

#include <cassert>
#include <cstdarg>

namespace logger {
enum Level {
    LEVEL_ERROR,
    LEVEL_WARNING,
    LEVEL_INFO,
    LEVEL_DEBUG,
};

/**
 * @brief Basic singleton logger with contextual logging support.
 *
 * This keeps printf-style formatting to minimize migration effort from the
 * previous macros while adding file, line, and function metadata.
 */
class Logger {
  public:
    static Logger* instance();
    void log(const Level level, const char* file, const int line,
             const char* function, const char* format, ...);

  private:
    static const char* level_to_string(const Level level);
    void vlog(const Level level, const char* file, const int line,
              const char* function, const char* format, va_list args);
};

}  // namespace logger

/**
 * @brief Macro for printing errors, drop-in replacement for printf with
 * additional information.
 */
#define SINUCA3_ERROR_PRINTF(...)                                         \
    do {                                                                  \
        logger::Logger::instance()->log(logger::LEVEL_ERROR, __FILE__,    \
                                        __LINE__, __func__, __VA_ARGS__); \
    } while (0)

/**
 * @brief Macro for printing warnings, drop-in replacement for printf with
 * additional information.
 */
#define SINUCA3_WARNING_PRINTF(...)                                       \
    do {                                                                  \
        logger::Logger::instance()->log(logger::LEVEL_WARNING, __FILE__,  \
                                        __LINE__, __func__, __VA_ARGS__); \
    } while (0)

/**
 * @brief Macro for printing general information, drop-in replacement for
 * printf.
 */
#define SINUCA3_LOG_PRINTF(...)                                           \
    do {                                                                  \
        logger::Logger::instance()->log(logger::LEVEL_INFO, __FILE__,     \
                                        __LINE__, __func__, __VA_ARGS__); \
    } while (0)

#ifndef NDEBUG
/**
 * @brief Macro for printing debug information. In debug builds, it's a drop-in
 * replacement for printf. In release builds, compiles to nothing.
 */
#define SINUCA3_DEBUG_PRINTF(...)                                         \
    do {                                                                  \
        logger::Logger::instance()->log(logger::LEVEL_DEBUG, __FILE__,    \
                                        __LINE__, __func__, __VA_ARGS__); \
    } while (0)
#else
/**
 * @brief Macro for printing debug information. In debug builds, it's a drop-in
 * replacement for printf. In release builds, compiles to nothing.
 */
#define SINUCA3_DEBUG_PRINTF(...) \
    do {                          \
    } while (0)
#endif  // SINUCA3_DEBUG_PRINTF

#endif  // SINUCA3_UTILS_LOGGER_HPP_
