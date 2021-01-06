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
    class BindingScope {
        public:
            BindingScope(BindingTo<T>& binding) : binding_(binding) { };

            void inSingletonScope() {

            }

        private:
            inversify::BindingTo<T>& binding_;
    };

    template <typename T>
    class BindingTo {
        public:
            BindingScope<T>&& toConstantValue(const T value) {
                inversify::ResolverPtr<T> resolver = std::make_shared<inversify::ConstantResolver<T>>(value);
                resolver_ = resolver;

                BindingScope<T> scope { *this };
                return std::move(scope);
            }

            BindingScope<T>&& toDynamicValue(inversify::Factory<T> factory) {
                BindingScope<T> scope { *this };
                return std::move(scope);
            }

        protected:
            inversify::ResolverPtr<T> resolver_;
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
