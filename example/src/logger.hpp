#pragma once

#include <iostream>

#include <mosure/inversify.hpp>

#include <api/logger.hpp>
#include <api/settings.hpp>
#include <api/symbols.hpp>


class Logger : public ILogger {
    public:
        Logger(ISettings settings)
            :
            settings_(settings)
        { }

        void info(std::string message) const override {
            log(LogLevel::INFO, message);
        };

        void warn(std::string message) const override {
            log(LogLevel::WARN, message);
        };

        void error(std::string message) const override {
            log(LogLevel::ERROR, message);
        };

        void log(LogLevel level, std::string message) const override {
            if (settings_.logLevel > level) {
                return;
            }

            std::string level_prefix;
            switch (level) {
                case LogLevel::INFO:
                    level_prefix = "INFO";
                    break;
                case LogLevel::WARN:
                    level_prefix = "WARN";
                    break;
                case LogLevel::ERROR:
                    level_prefix = "ERROR";
                    break;
            }

            std::cout << "[ExampleApp] [" << level_prefix << "] - " << message << std::endl;
        };

    private:
        ISettings settings_;
};

namespace inversify = mosure::inversify;
inline static auto injectLogger = inversify::Injectable<Logger>::inject(
    inversify::Inject<ISettings>(symbols::settings)
);
