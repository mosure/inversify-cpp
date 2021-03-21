#pragma once

#include <mosure/inversify.hpp>

#include <api/logger.hpp>
#include <api/symbols.hpp>


class MockLogger : public ILogger {
    public:
        void info(const std::string&) const override { };
        void warn(const std::string&) const override { };
        void error(const std::string&) const override { };
        void log(LogLevel, const std::string&) const override { };
};
