#pragma once

#include <cstdlib>

#include <mosure/inversify.hpp>

#include <api/settings.hpp>


struct Settings : ISettings {
    Settings() {
        logLevel = LogLevel::INFO;

        if(const char* level_str = std::getenv("LOG_LEVEL")) {
            auto val = std::string(level_str);

            if (val == "WARN") {
                logLevel = LogLevel::WARN;
            } else if (val == "ERROR") {
                logLevel = LogLevel::ERROR;
            }
        }
    }
};

namespace inversify = mosure::inversify;
inline static auto injectSettings = inversify::Injectable<Settings>::inject();
