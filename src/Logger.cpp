#include "Logger.hpp"

namespace utils
{
std::optional<Logger::LogLevel> Logger::parseLevel(std::string_view text)
{
    if (text == "debug")
        return LogLevel::DEBUG;
    if (text == "info")
        return LogLevel::INFO;
    if (text == "warning" || text == "warn")
        return LogLevel::WARNING;
    if (text == "error")
        return LogLevel::ERROR;
    return std::nullopt;
}

const char* Logger::toString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
    }
    return "UNKNOWN";
}

const char* Logger::colorFor(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG:
            return "\033[94m";  // Bright blue
        case LogLevel::INFO:
            return "\033[92m";  // Bright green
        case LogLevel::WARNING:
            return "\033[93m";  // Bright yellow
        case LogLevel::ERROR:
            return "\033[91m";  // Bright red
    }
    return RESET;
}
}  // namespace utils
