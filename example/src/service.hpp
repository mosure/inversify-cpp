#pragma once

#include <mosure/inversify.hpp>

#include <api/logger.hpp>
#include <api/service.hpp>
#include <api/types.hpp>


class Service : public IService {
    public:
        Service(ILoggerPtr logger)
            :
            logger_(logger)
        { }

        void run() override {
            logger_->info("Info message.");
            logger_->warn("Warn message.");
            logger_->error("Error message.");
        }

    private:
        ILoggerPtr logger_;
};

namespace inversify = mosure::inversify;
inline static auto injectService = inversify::Injectable<Service>::inject(
    inversify::Inject<ILoggerPtr>(types::logger)
);
