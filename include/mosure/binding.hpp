#pragma once

#include <functional>
#include <memory>

#include <mosure/factory.hpp>
#include <mosure/resolver.hpp>


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
            inline T resolve() const {
                if (!this->resolver_) {
                    throw inversify::exceptions::ResolutionException("inversify::Resolver not found. Malformed binding.");
                }

                return this->resolver_->resolve();
            }
    };

}
