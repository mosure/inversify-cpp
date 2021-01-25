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
            inversify::BindingTo<T>& bind(const inversify::Symbol<T>& type) {
                static_assert(!std::is_abstract<T>(), "inversify::Container cannot bind/get abstract class value (use a smart pointer instead).");

                auto binding = inversify::Binding(type);

                auto lookup = bindings_.find(type.id);
                if (lookup != bindings_.end()) {
                    bindings_.erase(type.id);
                }

                auto pair = std::make_pair(type.id, std::any(binding));
                bindings_.insert(pair);

                return std::any_cast<inversify::Binding<T>&>(bindings_.at(type.id));
            }

            template <typename T>
            T get(const inversify::Symbol<T>& type) const {
                auto symbolBinding = bindings_.find(type.id);
                if (symbolBinding == bindings_.end()) {
                    throw inversify::exceptions::SymbolException();
                }

                auto binding = std::any_cast<inversify::Binding<T>>(symbolBinding->second);

                return binding.resolve(context_);
            }

        private:
            std::unordered_map<int, std::any> bindings_ { };
            inversify::Context context_ { *this };
    };

}
