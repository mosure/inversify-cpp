#pragma once

#include <any>
#include <unordered_map>

#include <mosure/binding.hpp>
#include <mosure/context.hpp>
#include <mosure/symbol.hpp>
#include <mosure/exceptions/symbol.hpp>


namespace mosure::inversify {

    class Container {
        public:
            template <typename T>
            inversify::BindingToPtr<T> bind(const inversify::Symbol& type) {
                auto binding = std::make_shared<inversify::Binding<T>>(type);

                auto pair = std::make_pair(type, std::any(binding));
                bindings_.insert(pair);

                return binding;
            }

            template <typename T>
            T get(const inversify::Symbol& type) const {
                auto rawBinding = bindings_.find(type);
                if (rawBinding == bindings_.end()) {
                    throw inversify::exceptions::SymbolException(type);
                }

                auto binding = std::any_cast<BindingPtr<T>>(rawBinding->second);

                Context context { *this };
                return binding->resolve(context);
            }

        private:
            std::unordered_map<inversify::Symbol, std::any> bindings_ { };
    };

}
