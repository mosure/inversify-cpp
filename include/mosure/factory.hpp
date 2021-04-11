#pragma once

#include <functional>

#include <mosure/context.hpp>


namespace mosure::inversify {

template <typename T, typename... SymbolTypes>
using Factory = std::function<T(const inversify::Context<SymbolTypes...>&)>;

}
