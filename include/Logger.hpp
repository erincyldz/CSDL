#pragma once
#include <array>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace utils
{
class Logger
{
  public:
    // Ordered by severity: a message is printed when level >= current log level.
    enum class LogLevel
    {
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR
    };

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    void setLogLevel(LogLevel level)
    {
        m_logLevel = level;
    }
    LogLevel getLogLevel() const
    {
        return m_logLevel;
    }

    // Accepts "debug", "info", "warning"/"warn", "error" (case-sensitive, lower case).
    static std::optional<LogLevel> parseLevel(std::string_view text);
    static const char* toString(LogLevel level);

    template<typename... Args>
    void log(std::string_view className, LogLevel level, std::string_view format,
             const Args&... args)
    {
        if (level < m_logLevel)
            return;

        const std::string message = formatString(format, args...);

        std::lock_guard<std::mutex> lock(m_mutex);
        std::ostream& out = (level >= LogLevel::WARNING) ? std::cerr : std::cout;
        out << BOLD << RED << "[" << className << "]" << RESET << colorFor(level) << " ["
            << toString(level) << "]" << RESET << "\t-\t" << BOLD << WHITE << message << RESET
            << '\n';
        if (level >= LogLevel::WARNING)
            out.flush();
    }

  private:
    static constexpr const char* RESET = "\033[0m";
    static constexpr const char* BOLD = "\033[1m";
    static constexpr const char* RED = "\033[31m";
    static constexpr const char* WHITE = "\033[37m";

    LogLevel m_logLevel = LogLevel::INFO;
    std::mutex m_mutex;

    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static const char* colorFor(LogLevel level);

    // Replaces each "{}" in `format` with the next argument.
    template<typename... Args>
    static std::string formatString(std::string_view format, const Args&... args)
    {
        const std::array<std::string, sizeof...(Args)> argArray = {toString(args)...};
        std::ostringstream oss;
        std::size_t argIndex = 0;
        for (std::size_t i = 0; i < format.size(); ++i)
        {
            if (format[i] == '{' && i + 1 < format.size() && format[i + 1] == '}' &&
                argIndex < argArray.size())
            {
                oss << argArray[argIndex++];
                ++i;
            }
            else
            {
                oss << format[i];
            }
        }
        return oss.str();
    }

    template<typename T>
    static std::string toString(const T& value)
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            return value ? "true" : "false";
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            return std::to_string(value);
        }
        else
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    }
};
}  // namespace utils
