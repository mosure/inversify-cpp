#pragma once

#include <any>
#include <unordered_map>

#include <mosure/binding.hpp>
#include <mosure/exceptions/symbol.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    class Container {
        public:
            template <typename Interface>
            inversify::BindingTo<Interface>&& bind(inversify::Symbol& type) {
                Binding<Interface> binding { type };

                auto pair = std::make_pair(type, std::any(binding));

                bindings_.insert(pair);
            }

            template <typename Interface>
            Interface resolve(inversify::Symbol& type) {
                auto rawBinding = bindings_.find(type);

                if (rawBinding == bindings_.end()) {
                    throw inversify::exceptions::SymbolException();
                }

                auto binding = std::any_cast<Binding<Interface>>(rawBinding->second);

                return binding.resolve();
            }

        private:
            std::unordered_map<
                inversify::Symbol,
                std::any
            > bindings_ { };
    };

}
