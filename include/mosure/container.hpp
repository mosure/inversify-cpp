#pragma once

#include <typeinfo>
#include <map>
#include <utility>

#include <mosure/binding.hpp>
#include <mosure/context.hpp>
#include <mosure/exceptions/symbol.hpp>
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

            using BindingVariant = std::variant<
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

                auto binding = inversify::Binding<typename T::value, SymbolTypes...>();
                auto key = typeid(T).hash_code();

                auto lookup = bindings_.find(key);
                if (lookup != bindings_.end()) {
                    bindings_.erase(key);
                }

                auto pair = std::make_pair(key, binding);
                bindings_.insert(pair);

                return std::get<
                    inversify::Binding<typename T::value, SymbolTypes...>
                >(bindings_.at(key));
            }

            template <typename T>
            inline typename T::value get() const {
                static_assert(
                    meta::contains_v<T, SymbolTypes...>,
                    "inversify::Container symbol not registered"
                );

                auto key = typeid(T).hash_code();

                auto symbolBinding = bindings_.find(key);
                if (symbolBinding == bindings_.end()) {
                    throw inversify::exceptions::SymbolException();
                }

                auto binding = std::get<
                    inversify::Binding<typename T::value, SymbolTypes...>
                >(symbolBinding->second);

                return binding.resolve(context_);
            }

        private:
            std::map<std::size_t, BindingVariant> bindings_ {};
            inversify::Context<SymbolTypes...> context_ { *this };
    };

}
