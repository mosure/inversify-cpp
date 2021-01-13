#pragma once

#include <functional>


namespace mosure::inversify {

    struct Context;

    template <typename T>
    using Factory = std::function<T(const inversify::Context&)>;

}
