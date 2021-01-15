#pragma once

#include <any>
#include <unordered_map>
#include <utility>

#include <mosure/binding.hpp>
#include <mosure/context.hpp>
#include <mosure/symbol.hpp>
#include <mosure/exceptions/symbol.hpp>
#include <mosure/interfaces/icontainer.hpp>


namespace mosure::inversify {

    class Container : public inversify::IContainer<Container> {
        public:
            template <typename T>
            inversify::BindingTo<T>& bind(const inversify::Symbol& type) {
                static_assert(!std::is_abstract<T>(), "inversify::Container cannot bind/get abstract class value (use a smart pointer instead).");

                auto binding = inversify::Binding<T>(type);

                auto lookup = bindings_.find(type);
                if (lookup != bindings_.end()) {
                    bindings_.erase(type);
                }

                auto pair = std::make_pair(type, std::any(binding));
                bindings_.insert(pair);

                return std::any_cast<inversify::Binding<T>&>(bindings_.at(type));
            }

            template <typename T>
            T get(const inversify::Symbol& type) const {
                auto symbolBinding = bindings_.find(type);
                if (symbolBinding == bindings_.end()) {
                    throw inversify::exceptions::SymbolException(type);
                }

                auto binding = std::any_cast<Binding<T>>(symbolBinding->second);

                return binding.resolve(context_);
            }

        private:
            std::unordered_map<inversify::Symbol, std::any> bindings_ { };
            inversify::Context context_ { *this };
    };

}
