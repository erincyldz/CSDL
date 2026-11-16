#pragma once
#include "Logger.hpp"

#include <string>
#include <string_view>
#include <utility>

// Thin per-component front-end for the global Logger that prefixes messages with a name.
class ClassLogger
{
  public:
    using Level = utils::Logger::LogLevel;

    explicit ClassLogger(std::string className) : m_className(std::move(className)) {}

    template<typename... Args>
    void debug(std::string_view format, const Args&... args) const
    {
        utils::Logger::getInstance().log(m_className, Level::DEBUG, format, args...);
    }
    template<typename... Args>
    void info(std::string_view format, const Args&... args) const
    {
        utils::Logger::getInstance().log(m_className, Level::INFO, format, args...);
    }
    template<typename... Args>
    void warning(std::string_view format, const Args&... args) const
    {
        utils::Logger::getInstance().log(m_className, Level::WARNING, format, args...);
    }
    template<typename... Args>
    void error(std::string_view format, const Args&... args) const
    {
        utils::Logger::getInstance().log(m_className, Level::ERROR, format, args...);
    }

    const std::string& getClassName() const
    {
        return m_className;
    }

  private:
    std::string m_className;
};
