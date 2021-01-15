#pragma once

#include <memory>
#include <string>


enum LogLevel {
    INFO,
    WARN,
    ERROR
};

struct ILogger {
    virtual void info(const std::string& message) const = 0;
    virtual void warn(const std::string& message) const = 0;
    virtual void error(const std::string& message) const = 0;

    virtual void log(LogLevel level, const std::string& message) const = 0;
};

using ILoggerPtr = std::shared_ptr<ILogger>;
