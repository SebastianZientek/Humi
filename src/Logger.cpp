#include "Logger.hpp"

Logger::LogLevel Logger::currentLogLevel;
Logger::PrinterFunc Logger::printer = [](const std::string&){};