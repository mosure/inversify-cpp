#pragma once

#include <functional>
#include <memory>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/resolver.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename T>
    class BindingScope {
        public:
            void inSingletonScope() {
                resolver_ = std::make_shared<inversify::CachedResolver<T>>(resolver_);
            }

        protected:
            inversify::ResolverPtr<T> resolver_;
    };

    template <typename T>
    class BindingTo : public BindingScope<T> {
        public:
            void toConstantValue(T&& value) {
                this->resolver_ = std::make_shared<inversify::ConstantResolver<T>>(value);
            }

            BindingScope<T>& toDynamicValue(inversify::Factory<T> factory) {
                this->resolver_ = std::make_shared<inversify::DynamicResolver<T>>(factory);

                return *this;
            }

            template <typename U>
            BindingScope<T>& to() {
                this->resolver_ = std::make_shared<inversify::AutoResolver<T, U>>();

                return *this;
            }
    };

    template <typename T>
    class Binding : public BindingTo<T> {
        public:
            explicit Binding(const inversify::Symbol<T>& symbol)
                :
                symbol_(symbol)
            { }

            T resolve(const Context& context) const {
                if (!this->resolver_) {
                    throw inversify::exceptions::ResolutionException("inversify::Resolver not found. Malformed binding.");
                }

                return this->resolver_->resolve(context);
            }

        private:
            const inversify::Symbol<T>& symbol_;
    };

}
