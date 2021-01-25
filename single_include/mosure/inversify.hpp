/*
IoC and Dependency Injection for Modern C++
Version 1.0.0
https://github.com/mosure/inversifycpp

MIT License

Copyright (c) 2020 Mitchell Mosure

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <any>              // any, any_cast
#include <functional>       // function
#include <memory>           // make_shared, make_unique, shared_ptr, unique_ptr
#include <stdexcept>        // runtime_error
#include <string>           // string
#include <unordered_map>    // unordered_map
#include <tuple>            // make_from_tuple, tuple
#include <type_traits>      // apply, conjunction_v, false_type, true_type
#include <utility>          // make_pair

// #include <mosure/binding.hpp>


#include <functional>
#include <memory>

// #include <mosure/context.hpp>


// #include <mosure/interfaces/icontainer.hpp>


#include <memory>

// #include <mosure/symbol.hpp>


#include <iostream>


namespace mosure::inversify {

    namespace {
        inline static int counter = 0;
    }

    template <typename Interface>
    struct Symbol {
        Symbol() : id(counter++) { }

        const int id;
        using value = Interface;
    };

}



namespace mosure::inversify {

    template <typename T>
    class BindingTo;

    template <typename Implementation>
    class IContainer {
        public:
            template <typename T>
            inversify::BindingTo<T>& bind(const inversify::Symbol<T>& type) {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->bind(type);
            }

            template <typename T>
            T get(const inversify::Symbol<T>& type) const {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->get(type);
            }
    };

}



namespace mosure::inversify {

    class Container;

    struct Context {
        const inversify::IContainer<Container>& container;
    };

}

// #include <mosure/factory.hpp>


#include <functional>

// #include <mosure/context.hpp>



namespace mosure::inversify {

    template <typename T>
    using Factory = std::function<T(const inversify::Context&)>;

}

// #include <mosure/resolver.hpp>


#include <atomic>
#include <memory>
#include <type_traits>

// #include <mosure/context.hpp>

// #include <mosure/factory.hpp>

// #include <mosure/injectable.hpp>


#include <memory>
#include <tuple>
#include <type_traits>

// #include <mosure/context.hpp>

// #include <mosure/factory.hpp>

// #include <mosure/meta.hpp>


#include <type_traits>
#include <utility>


namespace mosure::inversify::meta {

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type { };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type { };

}

// #include <mosure/symbol.hpp>



namespace mosure::inversify {

    template <typename ...Types>
    inline constexpr bool valid_inject_types_v = std::conjunction_v<
        meta::is_specialization<Types, Symbol>...
    >;

    template <typename Implementation>
    class Injectable {
        public:
            inline static inversify::Factory<Implementation> factory;
            inline static inversify::Factory<std::unique_ptr<Implementation>> factory_unique;
            inline static inversify::Factory<std::shared_ptr<Implementation>> factory_shared;

            template <typename... Dependencies>
            inline static Injectable inject(Dependencies... dependencies) {
                static_assert(valid_inject_types_v<Dependencies...>, "inversify::Injectable dependencies must be of type inversify::Symbol");

                factory = [
                    deps = std::make_tuple(dependencies...)
                ](const inversify::Context& context) mutable {
                    auto expansion = [&context](auto&& ... deps){
                        auto args = std::make_tuple(resolve_dependency(context, deps)...);

                        return std::make_from_tuple<Implementation>(args);
                    };

                    return std::apply(expansion, std::move(deps));
                };

                factory_unique = [
                    deps = std::make_tuple(dependencies...)
                ](const inversify::Context& context) mutable {
                    auto expansion = [&context](auto&& ... deps){
                        return std::make_unique<Implementation>(resolve_dependency(context, deps)...);
                    };

                    return std::apply(expansion, std::move(deps));
                };

                factory_shared = [
                    deps = std::make_tuple(dependencies...)
                ](const inversify::Context& context) mutable {
                    auto expansion = [&context](auto&& ... deps){
                        return std::make_shared<Implementation>(resolve_dependency(context, deps)...);
                    };

                    return std::apply(expansion, std::move(deps));
                };

                return Injectable {};
            }

        private:
            template <typename Dependency>
            inline static typename Dependency::value resolve_dependency(const inversify::Context& context, Dependency dep) {
                using Interface = typename Dependency::value;
                auto symbol = static_cast<Symbol<Interface>>(dep);

                return context.container.get(symbol);
            }
    };

}

// #include <mosure/exceptions/resolution.hpp>


#include <stdexcept>
#include <string>


namespace mosure::inversify::exceptions {

    struct ResolutionException : public std::runtime_error {
        explicit ResolutionException(const std::string& msg) : std::runtime_error(msg) { }
    };

}



namespace mosure::inversify {

    template <typename T>
    class Resolver {
        public:
            virtual T resolve(const inversify::Context&) = 0;
    };

    template <typename T>
    using ResolverPtr = std::shared_ptr<Resolver<T>>;

    template <typename T>
    class ConstantResolver : public Resolver<T> {
        public:
            explicit ConstantResolver(T value) : value_(value) { }

            T resolve(const inversify::Context&) override {
                return value_;
            }

        private:
            T value_;
    };

    template <typename T>
    class DynamicResolver : public Resolver<T> {
        public:
            explicit DynamicResolver(inversify::Factory<T> factory) : factory_(factory) { }

            T resolve(const inversify::Context& context) override {
                return factory_(context);
            }

        private:
            inversify::Factory<T> factory_;
    };

    template <typename T, typename U, typename = void>
    class AutoResolver;

    template <typename T, typename U>
    class AutoResolver<T, U> : public Resolver<T> {
        public:
            T resolve(const inversify::Context& context) override {
                if (!inversify::Injectable<U>::factory_unique) {
                    throw inversify::exceptions::ResolutionException("inversify::AutoResolver could not find factory. Is the binding correct?");
                }

                return inversify::Injectable<U>::factory(context);
            }
    };

    // unique_ptr specialization
    template <
        typename T,
        typename U
    >
    class AutoResolver<std::unique_ptr<T>, U> : public Resolver<std::unique_ptr<T>> {
        public:
            std::unique_ptr<T> resolve(const inversify::Context& context) override {
                if (!inversify::Injectable<U>::factory_unique) {
                    throw inversify::exceptions::ResolutionException("inversify::AutoResolver could not find unique_ptr factory. Is the binding correct?");
                }

                return inversify::Injectable<U>::factory_unique(context);
            }
    };

    // shared_ptr specialization
    template <
        typename T,
        typename U
    >
    class AutoResolver<std::shared_ptr<T>, U> : public Resolver<std::shared_ptr<T>> {
        public:
            std::shared_ptr<T> resolve(const inversify::Context& context) override {
                if (!inversify::Injectable<U>::factory_shared) {
                    throw inversify::exceptions::ResolutionException("inversify::AutoResolver could not find shared_ptr factory. Is the binding correct?");
                }

                return inversify::Injectable<U>::factory_shared(context);
            }
    };

    template <typename T>
    class CachedResolver : public Resolver<T> {
        static_assert(std::is_copy_constructible_v<T>, "inversify::CachedResolver requires a copy constructor. Are you caching a unique_ptr?");

        public:
            explicit CachedResolver(ResolverPtr<T> parent) : parent_(parent) { }

            T resolve(const inversify::Context& context) override {
                if (!hasCached_) {
                    hasCached_ = true;
                    cached_ = parent_->resolve(context);
                }

                return cached_;
            }

        private:
            T cached_;
            std::atomic<bool> hasCached_ { false };
            ResolverPtr<T> parent_;
    };

}

// #include <mosure/symbol.hpp>



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

// #include <mosure/container.hpp>


#include <any>
#include <unordered_map>
#include <utility>

// #include <mosure/binding.hpp>

// #include <mosure/context.hpp>

// #include <mosure/symbol.hpp>

// #include <mosure/exceptions/symbol.hpp>


#include <stdexcept>


namespace mosure::inversify::exceptions {

    struct SymbolException : public std::runtime_error {
        explicit SymbolException() : std::runtime_error("inversify::Symbol not found.") { }
    };

}

// #include <mosure/interfaces/icontainer.hpp>



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

// #include <mosure/context.hpp>

// #include <mosure/factory.hpp>

// #include <mosure/injectable.hpp>

// #include <mosure/meta.hpp>

// #include <mosure/resolver.hpp>

// #include <mosure/symbol.hpp>


// #include <mosure/exceptions/resolution.hpp>

// #include <mosure/exceptions/symbol.hpp>



/**
    @brief namespace for Mitchell Mosure - InversifyCpp
    @see https://github.com/mosure
    @since version 1.0.0
*/
namespace mosure::inversify { }
