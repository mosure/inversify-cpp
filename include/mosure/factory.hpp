#pragma once

#include <functional>


namespace mosure::inversify {

    template <typename T>
    using Factory = std::function<T()>;

}
