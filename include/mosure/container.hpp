#pragma once

#include <tuple>
#include <typeinfo>

#include <mosure/binding.hpp>
#include <mosure/context.hpp>
#include <mosure/interfaces/icontainer.hpp>
#include <mosure/meta.hpp>


namespace mosure::inversify {

    using KeyType = int;

    template <typename... SymbolTypes>
    class Container
        :
        public inversify::IContainer<Container, SymbolTypes...> 
    {
        public:
            static_assert(
                meta::valid_symbol_types_v<SymbolTypes...>,
                "inversify::Container symbols must be of type inversify::Symbol"
            );

            static_assert(
                !meta::is_empty_v<SymbolTypes...>,
                "inversify::Container must register at least one symbol"
            );

            using BindingMap = std::tuple<
                inversify::Binding<
                    typename SymbolTypes::value,
                    SymbolTypes...
                >...
            >;

            template <typename T>
            inline inversify::BindingTo<typename T::value, SymbolTypes...>& bind() {
                static_assert(
                    meta::contains_v<T, SymbolTypes...>,
                    "inversify::Container symbol not registered"
                );

                return std::get<
                    inversify::Binding<typename T::value, SymbolTypes...>
                >(bindings_);
            }

            template <typename T>
            inline typename T::value get() const {
                static_assert(
                    meta::contains_v<T, SymbolTypes...>,
                    "inversify::Container symbol not registered"
                );

                return std::get<
                    inversify::Binding<typename T::value, SymbolTypes...>
                >(bindings_).resolve(context_);
            }

        private:
            BindingMap bindings_ {};
            inversify::Context<SymbolTypes...> context_ { *this };
    };

}
