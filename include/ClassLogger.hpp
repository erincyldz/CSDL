#pragma once
#include "Logger.hpp"

#include <string>
using Logger = utils::Logger;
class ClassLogger
{
  public:
    ClassLogger(const std::string& className) : className(className) {}

    template<typename... Args>
    void info(const std::string& format, Args... args)
    {
        Logger::getInstance().log(className, Logger::LogLevel::INFO, format, args...);
    }

    template<typename... Args>
    void warning(const std::string& format, Args... args)
    {
        Logger::getInstance().log(className, Logger::LogLevel::WARNING, format, args...);
    }

    template<typename... Args>
    void error(const std::string& format, Args... args)
    {
        Logger::getInstance().log(className, Logger::LogLevel::ERROR, format, args...);
    }

    template<typename... Args>
    void debug(const std::string& format, Args... args)
    {
        Logger::getInstance().log(className, Logger::LogLevel::DEBUG, format, args...);
    }

    std::string getClassName() const
    {
        return className;
    }

  private:
    std::string className;
};
