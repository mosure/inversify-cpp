#pragma once

#include <any>
#include <unordered_map>
#include <utility>

#include <mosure/binding.hpp>
#include <mosure/context.hpp>
#include <mosure/symbol.hpp>
#include <mosure/exceptions/symbol.hpp>


namespace mosure::inversify {

    class Container {
        public:
            template <typename T>
            inversify::BindingToPtr<T> bind(const inversify::Symbol& type) {
                static_assert(!std::is_abstract<T>(), "inversify::Container cannot bind/get abstract class value (use a smart pointer instead).");

                auto binding = std::make_shared<inversify::Binding<T>>(type);

                auto lookup = bindings_.find(type);
                if (lookup != bindings_.end()) {
                    bindings_.erase(type);
                }

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

                return binding->resolve(context_);
            }

        private:
            std::unordered_map<inversify::Symbol, std::any> bindings_ { };
            Context context_ { *this };
    };

}
