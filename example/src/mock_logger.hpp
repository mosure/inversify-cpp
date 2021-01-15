#pragma once

#include <mosure/inversify.hpp>

#include <api/logger.hpp>
#include <api/symbols.hpp>


class MockLogger : public ILogger {
    public:
        void info(std::string) const override { };
        void warn(std::string) const override { };
        void error(std::string) const override { };
        void log(LogLevel, std::string) const override { };
};

namespace inversify = mosure::inversify;
inline static auto injectMockLogger = inversify::Injectable<MockLogger>::inject();
