#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/injectable.hpp>
#include <mosure/meta.hpp>


namespace mosure::inversify {

    template <
        typename T,
        typename U,
        typename... SymbolTypes
    >
    inline static inversify::Factory<T, SymbolTypes...> get_auto_resolver() {
        if constexpr (meta::is_unique_ptr<T>::value) {
            return [](const inversify::Context<SymbolTypes...>& context) {
                auto expansion = [](auto&& ... deps){
                    return std::make_unique<U>(deps...);
                };

                return std::apply(
                    expansion,
                    std::move(
                        inversify::Injectable<U>::resolve(context)
                    )
                );
            };
        } else if constexpr (meta::is_shared_ptr<T>::value) {
            return [](const inversify::Context<SymbolTypes...>& context) {
                auto expansion = [](auto&& ... deps){
                    return std::make_shared<U>(deps...);
                };

                return std::apply(
                    expansion,
                    std::move(
                        inversify::Injectable<U>::resolve(context)
                    )
                );
            };
        } else {
            return [](const inversify::Context<SymbolTypes...>& context) {
                return std::make_from_tuple<U>(
                    inversify::Injectable<U>::resolve(context)
                );
            };
        }
    }
}
