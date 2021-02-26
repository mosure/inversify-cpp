#pragma once

#include <any>
#include <typeinfo>
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
            inversify::BindingTo<typename T::value>& bind() {
                auto binding = inversify::Binding<typename T::value>();

                auto key = typeid(T).hash_code();

                auto lookup = bindings_.find(key);
                if (lookup != bindings_.end()) {
                    bindings_.erase(key);
                }

                auto pair = std::make_pair(key, std::any(binding));
                bindings_.insert(pair);

                return std::any_cast<inversify::Binding<typename T::value>&>(bindings_.at(key));
            }

            template <typename T>
            typename T::value get() const {
                auto key = typeid(T).hash_code();

                auto symbolBinding = bindings_.find(key);
                if (symbolBinding == bindings_.end()) {
                    throw inversify::exceptions::SymbolException();
                }

                auto binding = std::any_cast<inversify::Binding<typename T::value>>(symbolBinding->second);

                return binding.resolve(context_);
            }

        private:
            std::unordered_map<std::size_t, std::any> bindings_ { };
            inversify::Context context_ { *this };
    };

}
