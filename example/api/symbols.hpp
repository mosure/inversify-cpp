#pragma once

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;
namespace symbols {
    inline const inversify::Symbol logger { "Logger" };
    inline const inversify::Symbol service { "Service" };
    inline const inversify::Symbol settings { "Settings" };
}
