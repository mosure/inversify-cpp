#pragma once

#include <memory>

#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename T, typename... SymbolTypes>
    class BindingTo;

    template <template <typename...> class Implementation, typename... SymbolTypes>
    class IContainer {
        public:
            template <typename T>
            inversify::BindingTo<typename T::value, SymbolTypes...>& bind() {
                auto crtpImplementation = static_cast<Implementation<SymbolTypes...> const *>(this);

                return crtpImplementation->template bind<T>();
            }

            template <typename T>
            typename T::value get() const {
                auto crtpImplementation = static_cast<Implementation<SymbolTypes...> const *>(this);

                return crtpImplementation->template get<T>();
            }
    };

}
