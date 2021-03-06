#pragma once

#include "fmt/format.h"

namespace forrest {

enum class LogLevel
{
    info,
    verbose,
    debug,
    trace
};

struct LogGlobals
{
    const char* program_name = "";
    LogLevel level = LogLevel::info;
};

extern LogGlobals g_log;

template <typename... Args>
[[noreturn]] void log_fatal(const char* format, const Args&... args)
{
    fprintf(stderr, "%s: error: ", g_log.program_name);
    fmt::print(stderr, format, args...);
    fprintf(stderr, "\n");
    std::exit(EXIT_FAILURE);
}

template <typename... Args>
void report_error(const char* format, const Args&... args)
{
    fprintf(stderr, "%s: error: ", g_log.program_name);
    fmt::print(stderr, format, args...);
    fprintf(stderr, "\n");
}

inline void report_error(const std::string& msg)
{
    fmt::print(stderr, "{}: error: {}\n", g_log.program_name, msg);
}

#define LOG_DEBUG(format, ...) \
    (LogLevel::debug <= g_log.level ? log_debug(format, __VA_ARGS__) : (void)0)

template <typename... Args>
void log_debug(const char* format, const Args&... args)
{
    fmt::print(stderr, "{}: debug: {}\n", g_log.program_name, fmt::format(format, args...));
}
}  // namespace forrest
