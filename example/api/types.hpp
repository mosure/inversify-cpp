#pragma once

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;
namespace types {
    inline const inversify::Symbol logger { "Logger" };
    inline const inversify::Symbol service { "Service" };
    inline const inversify::Symbol settings { "Settings" };
}
