#pragma once

#include <memory>
#include <string>


enum LogLevel {
    INFO,
    WARN,
    ERROR
};

struct ILogger {
    virtual void info(std::string message) const = 0;
    virtual void warn(std::string message) const = 0;
    virtual void error(std::string message) const = 0;

    virtual void log(LogLevel level, std::string message) const = 0;
};

using ILoggerPtr = std::shared_ptr<ILogger>;
