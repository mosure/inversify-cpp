#pragma once

#include <functional>
#include <memory>

#include <mosure/factory.hpp>
#include <mosure/resolver.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename T>
    class Binding;

    template <typename T>
    class BindingTo;

    template <typename T>
    class BindingScope : public std::enable_shared_from_this<BindingScope<T>> {
        public:
            void inSingletonScope() {
                resolver_ = std::make_shared<inversify::CachedResolver<T>>(resolver_);
            }

        protected:
            inversify::ResolverPtr<T> resolver_;
    };

    template <typename T>
    using BindingScopePtr = std::shared_ptr<BindingScope<T>>;

    template <typename T>
    class BindingTo : public BindingScope<T> {
        public:
            void toConstantValue(const T value) {
                resolver_ = std::make_shared<inversify::ConstantResolver<T>>(value);
            }

            BindingScopePtr<T> toDynamicValue(inversify::Factory<T> factory) {
                resolver_ = std::make_shared<inversify::DynamicResolver<T>>(factory);

                return shared_from_this();
            }
    };

    template <typename T>
    using BindingToPtr = std::shared_ptr<BindingTo<T>>;

    template <typename T>
    class Binding : public BindingTo<T> {
        public:
            Binding(const inversify::Symbol& symbol)
                :
                symbol_(symbol)
            { }

            T resolve(const Context& context) const {
                if (!resolver_) {
                    throw inversify::exceptions::ResolutionException("inversify::Resolver not found. Malformed binding: " + symbol_);
                }

                return resolver_->resolve(context);
            }

        private:
            const inversify::Symbol& symbol_;
    };

    template <typename T>
    using BindingPtr = std::shared_ptr<Binding<T>>;

}
