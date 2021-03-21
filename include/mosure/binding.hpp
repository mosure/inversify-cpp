#pragma once

#include <functional>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/resolver.hpp>
#include <mosure/exceptions/resolution.hpp>


namespace mosure::inversify {

    template <typename T, typename... SymbolTypes>
    class BindingScope {
        public:
            void inSingletonScope() {
                static_assert(
                    std::is_copy_constructible<T>::value,
                    "inversify::BindingScope singleton must have copy constructor"
                );

                this->factory_ = [this, factory = std::move(factory_)](auto& context) {
                    if (!this->cached_set_) {
                        this->cached_ = factory(context);
                        this->cached_set_ = true;
                    }

                    return this->cached_;
                };
            }

        protected:
            T cached_;
            bool cached_set_ { false };
            inversify::Factory<T, SymbolTypes...> factory_;
    };

    template <typename T, typename... SymbolTypes>
    class BindingTo
        : public BindingScope<T, SymbolTypes...>
    {
        public:
            void toConstantValue(T&& value) {
                this->factory_ = [val = std::move(value)](auto&) {
                    return val;
                };
            }

            BindingScope<T, SymbolTypes...>& toDynamicValue(inversify::Factory<T, SymbolTypes...>&& factory) {
                this->factory_ = std::move(factory);

                return *this;
            }

            template <typename U>
            BindingScope<T, SymbolTypes...>& to() {
                this->factory_ = inversify::get_auto_resolver<T, U, SymbolTypes...>();

                return *this;
            }
    };

    template <typename T, typename... SymbolTypes>
    class Binding
        : public BindingTo<typename T::value, SymbolTypes...>
    {
        public:
            inline typename T::value resolve(const inversify::Context<SymbolTypes...>& context) {
                return this->factory_(context);
            }
    };

}
