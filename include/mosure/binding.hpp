#pragma once

#include <functional>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/resolver.hpp>
#include <mosure/exceptions/resolution.hpp>


namespace mosure::inversify {

    enum BindingState {
        unique,
        singleton_uninitialized,
        singleton_initialized
    };

    template <typename T, typename... SymbolTypes>
    class BindingScope {
        public:
            void inSingletonScope() {
                state_ = BindingState::singleton_uninitialized;
            }

        protected:
            T cached_;
            inversify::Factory<T, SymbolTypes...> factory_;
            bool initialized_ { false };
            BindingState state_ { BindingState::unique };
    };

    template <typename T, typename... SymbolTypes>
    class BindingTo
        : public BindingScope<T, SymbolTypes...>
    {
        public:
            void toConstantValue(T&& value) {
                this->state_ = BindingState::singleton_initialized;
                this->cached_ = std::move(value);
                this->initialized_ = true;
            }

            BindingScope<T, SymbolTypes...>& toDynamicValue(inversify::Factory<T, SymbolTypes...> factory) {
                this->factory_ = factory;
                this->initialized_ = true;

                return *this;
            }

            template <typename U>
            BindingScope<T, SymbolTypes...>& to() {
                this->factory_ = inversify::get_auto_resolver<T, U, SymbolTypes...>();
                this->initialized_ = true;

                return *this;
            }
    };

    template <typename T, typename... SymbolTypes>
    class Binding
        : public BindingTo<typename T::value, SymbolTypes...>
    {
        public:
            inline typename T::value resolve(const inversify::Context<SymbolTypes...>& context) {
                if (!this->initialized_) {
                    throw inversify::exceptions::ResolutionException("inversify::Resolver not found. Malformed binding.");
                }

                if constexpr (std::is_copy_constructible<typename T::value>::value) {
                    switch (this->state_) {
                        case BindingState::singleton_uninitialized:
                            this->cached_ = this->factory_(context);
                            this->state_ = BindingState::singleton_initialized;
                        case BindingState::singleton_initialized:
                            return this->cached_;
                    }
                }

                return this->factory_(context);
            }
    };

}
