#pragma once

#include <functional>
#include <utility>

#include <mosure/context.hpp>
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
            BindingScope(BindingTo<T>& binding)
                :
                binding_(binding)
            { };

            void inSingletonScope() {

            }

        private:
            inversify::BindingTo<T>& binding_;
    };

    template <typename T>
    class BindingTo {
        public:
            BindingScope<T>&& toConstantValue(T value) {
                BindingScope<T> scope { *this };

                return std::move(scope);
            }

            template <typename U>
            BindingScope<T>&& toDynamicValue(std::function<U(inversify::Context)> factory) {
                BindingScope<T> scope { *this };

                return std::move(scope);
            }
    };

    template <typename T>
    class Binding : public BindingTo<T> {
        public:
            Binding(inversify::Symbol& symbol)
                :
                resolver_(),
                symbol_(symbol)
            { }

            T resolve() {
                throw "Resolve not implemented.";
            }

        protected:
            inversify::Resolver<T> resolver_;
            inversify::Symbol& symbol_;
    };

}
