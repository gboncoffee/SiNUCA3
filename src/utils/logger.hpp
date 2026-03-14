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
    LEVEL_ERROR = 0,
    LEVEL_WARNING = 1,
    LEVEL_INFO = 2,
#ifndef NDEBUG
    LEVEL_DEBUG = 3,
#endif
};

/**
 * @brief Basic singleton logger with contextual logging support.
 *
 * @details This keeps printf-style formatting to minimize migration effort from
 * the previous macros while adding file, line, and function metadata.
 */
class Logger {
  private:
    /* Minimum level that will be emitted. Messages with level > min_level_
     * are filtered out. */
    Level minLevel;

    /* @brief Default-construct with no filtering (show all levels). */
    Logger()
        :
#ifndef NDEBUG
          minLevel(LEVEL_DEBUG)
#else
          minLevel(LEVEL_INFO)
#endif
    {
    }

    /* @brief Get's a static string with the name of the log level. */
    static const char* Level2String(const Level level);
    /* @brief Logs. */
    void Vlog(const Level level, const char* file, const int line,
              const char* function, const char* format, va_list args);

  public:
    static Logger* Instance();

    /* @brief Don't use directly, prefer using the macros. */
    void Log(const Level level, const char* file, const int line,
             const char* function, const char* format, ...);

    /** @brief Don't use directly, prefer using the macro. */
    int SetLevel(const Level level);
    /** @brief Don't use directly, prefer using the macro. */
    Level GetLevel() const;
};

}  // namespace logger

/**
 * @brief Sets the log level.
 *
 * @details Messages with a level greater than the configured level will be
 * suppressed. Levels are ordered from most severe (LEVEL_ERROR) to most
 * verbose (LEVEL_DEBUG). The default is LEVEL_DEBUG (no filtering).
 */
#define SINUCA3_SET_LOG_LEVEL(level) \
    (logger::Logger::Instance()->SetLevel(level))

#define SINUCA3_GET_LOG_LEVEL() (logger::Logger::Instance()->GetLevel())

/**
 * @brief Macro for printing errors, drop-in replacement for printf with
 * additional information.
 */
#define SINUCA3_ERROR_PRINTF(...)                                         \
    do {                                                                  \
        logger::Logger::Instance()->Log(logger::LEVEL_ERROR, __FILE__,    \
                                        __LINE__, __func__, __VA_ARGS__); \
    } while (0)

/**
 * @brief Macro for printing warnings, drop-in replacement for printf with
 * additional information.
 */
#define SINUCA3_WARNING_PRINTF(...)                                       \
    do {                                                                  \
        logger::Logger::Instance()->Log(logger::LEVEL_WARNING, __FILE__,  \
                                        __LINE__, __func__, __VA_ARGS__); \
    } while (0)

/**
 * @brief Macro for printing general information, drop-in replacement for
 * printf.
 */
#define SINUCA3_LOG_PRINTF(...)                                           \
    do {                                                                  \
        logger::Logger::Instance()->Log(logger::LEVEL_INFO, __FILE__,     \
                                        __LINE__, __func__, __VA_ARGS__); \
    } while (0)

#ifndef NDEBUG
/**
 * @brief Macro for printing debug information. In debug builds, it's a drop-in
 * replacement for printf. In release builds, compiles to nothing.
 */
#define SINUCA3_DEBUG_PRINTF(...)                                         \
    do {                                                                  \
        logger::Logger::Instance()->Log(logger::LEVEL_DEBUG, __FILE__,    \
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
