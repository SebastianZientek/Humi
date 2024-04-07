#include "Logger.hpp"

Logger::LogLevel Logger::currentLogLevel;                          // NOLINT
Logger::PrinterFunc Logger::printer = [](const std::string &) {};  // NOLINT
