#pragma once
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
namespace utils
{
class Logger
{
  public:
    enum class LogLevel
    {
        DEBUG,
        ERROR,
        WARNING,
        INFO
    };

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    void setLogLevel(LogLevel level);
    Logger::LogLevel parseLogLevel(int argc, char* argv[])
    {
        if (argc > 1)
        {
            std::string arg = argv[1];
            if (arg == "-i")
            {
                return Logger::LogLevel::INFO;
            }
            else if (arg == "-w")
            {
                return Logger::LogLevel::WARNING;
            }
            else if (arg == "-e")
            {
                return Logger::LogLevel::ERROR;
            }
            else if (arg == "-d")
            {
                return Logger::LogLevel::DEBUG;
            }
            else
            {
                std::cerr << "Unknown log level argument: " << arg << std::endl;
                std::cerr << "Usage: " << argv[0] << " [-i | -w | -e]" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }

        return Logger::LogLevel::INFO;  // Default log level
    }

    template<typename... Args>
    void log(const std::string& className, LogLevel level, const std::string& format, Args... args)
    {
        if (level < m_logLevel)
            return;

        std::lock_guard<std::mutex> lock(mtx);

        std::string message = formatString(format, args...);
        std::cout << "[" << className << "] [" << logLevelToString(level) << "] " << message
                  << std::endl;
    }

  private:
    LogLevel m_logLevel = LogLevel::INFO;
    std::mutex mtx;

    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    template<typename... Args>
    std::string formatString(const std::string& format, Args... args)
    {
        std::ostringstream oss;
        const char* ptr = format.c_str();
        std::array<std::string, sizeof...(Args)> argArray = {toString(args)...};
        size_t argIndex = 0;

        while (*ptr)
        {
            if (*ptr == '{' && *(ptr + 1) == '}' && argIndex < argArray.size())
            {
                oss << argArray[argIndex++];
                ptr += 2;  // Skip "{}"
            }
            else
            {
                oss << *ptr++;
            }
        }

        return oss.str();
    }

    template<typename T>
    std::string toString(const T& value)
    {
        if constexpr (std::is_arithmetic<T>::value)
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

    std::string logLevelToString(LogLevel level) const;
};
}  // namespace utils