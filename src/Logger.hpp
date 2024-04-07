#pragma once

#include <functional>

#undef B1
#include <fmt/core.h>

class Logger
{
public:
    using PrinterFunc = std::function<void(const std::string &)>;

    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static void init(
        const PrinterFunc &printerFunc = [](const std::string &) {},
        LogLevel initialLogLevel = LogLevel::DEBUG)
    {
        printer = printerFunc;
        setLogLevel(initialLogLevel);
    }

    static void setLogLevel(LogLevel level)
    {
        currentLogLevel = level;
    }

    template <typename... Args>
    static void debug(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::DEBUG, "[DBG]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::INFO, "[INF]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warning(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::WARNING, "[WRN]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::ERROR, "[ERR]", format, std::forward<Args>(args)...);
    }

private:
    static LogLevel currentLogLevel;  // NOLINT
    static PrinterFunc printer;       // NOLINT

    template <typename... Args>
    static void log(LogLevel level,
                    const char *levelStr,
                    fmt::format_string<Args...> format,
                    Args &&...args)
    {
        if (level >= currentLogLevel)
        {
            std::string message
                = fmt::format("{} {}", levelStr, fmt::format(format, std::forward<Args>(args)...));
            printer(message);
        }
    }
};
