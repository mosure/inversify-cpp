#pragma once

#include <mosure/inversify.hpp>

#include <api/logger.hpp>
#include <api/service.hpp>
#include <api/settings.hpp>


namespace inversify = mosure::inversify;
namespace symbols {
    inline extern inversify::Symbol<ILoggerPtr> logger {};
    inline extern inversify::Symbol<IServicePtr> service {};
    inline extern inversify::Symbol<ISettings> settings {};
}
