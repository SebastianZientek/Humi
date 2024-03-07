#pragma once

#include <Arduino.h>

#undef B1
#include <fmt/core.h>

class Logger {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static void init(LogLevel initialLogLevel = LogLevel::DEBUG) {
      Serial1.begin(9600);
        // Initialize WiFi if needed
        // WiFi.begin("SSID", "password");
        setLogLevel(initialLogLevel);
    }

    static void setLogLevel(LogLevel level) {
        currentLogLevel = level;
    }

    template <typename... Args>
    static void debug(fmt::format_string<Args...> format, Args&&... args) {
        log(LogLevel::DEBUG, "[DBG]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(fmt::format_string<Args...> format, Args&&... args) {
        log(LogLevel::INFO, "[INF]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warning(fmt::format_string<Args...> format, Args&&... args) {
        log(LogLevel::WARNING, "[WRN]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(fmt::format_string<Args...> format, Args&&... args) {
        log(LogLevel::ERROR, "[ERR]", format, std::forward<Args>(args)...);
    }

private:
    static LogLevel currentLogLevel;

    template <typename... Args>
    static void log(LogLevel level, const char* levelStr, fmt::format_string<Args...> format, Args&&... args) {
        if (level >= currentLogLevel) {
            std::string message = fmt::format("{} {}", levelStr, fmt::format(format, std::forward<Args>(args)...));
            // std::cout << message;
            Serial1.println(message.c_str());
        }
    }
};

Logger::LogLevel Logger::currentLogLevel;