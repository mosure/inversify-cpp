#pragma once

#include <mosure/inversify.hpp>

#include <api/logger.hpp>
#include <api/service.hpp>
#include <api/settings.hpp>


namespace inversify = mosure::inversify;
namespace symbols {
    using logger = inversify::Symbol<ILoggerPtr>;
    using service = inversify::Symbol<IServicePtr>;
    using settings = inversify::Symbol<ISettings>;
}
