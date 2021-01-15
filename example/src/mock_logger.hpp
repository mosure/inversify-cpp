#pragma once

#include <mosure/inversify.hpp>

#include <api/logger.hpp>
#include <api/settings.hpp>
#include <api/types.hpp>


class MockLogger : public ILogger {
    public:
        void info(std::string message) const override { };
        void warn(std::string message) const override { };
        void error(std::string message) const override { };
        void log(LogLevel level, std::string message) const override { };
};

namespace inversify = mosure::inversify;
inline static auto injectMockLogger = inversify::Injectable<MockLogger>::inject();
