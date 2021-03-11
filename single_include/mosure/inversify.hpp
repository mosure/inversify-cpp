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

#include <functional>       // function
#include <memory>           // make_shared, make_unique, shared_ptr, unique_ptr
#include <stdexcept>        // runtime_error
#include <string>           // string
#include <tuple>            // make_from_tuple, tuple
#include <type_traits>      // apply, conjunction_v, disjunction_v, false_type, is_same, true_type

// #include <mosure/binding.hpp>


#include <functional>
#include <memory>

// #include <mosure/context.hpp>


// #include <mosure/interfaces/icontainer.hpp>


#include <memory>

// #include <mosure/symbol.hpp>


#include <type_traits>
#include <variant>


namespace mosure::inversify {

    template <typename Interface>
    struct Symbol {
        static_assert(
            !std::is_abstract<Interface>(),
            "inversify::Container cannot bind/get abstract class value (use a smart pointer instead)."
        );

        using value = Interface;
    };

}



namespace mosure::inversify {

    template <typename T, typename... SymbolTypes>
    class BindingTo;

    template <template <typename...> class Implementation, typename... SymbolTypes>
    class IContainer {
        public:
            template <typename T>
            inversify::BindingTo<typename T::value, SymbolTypes...>& bind() {
                auto crtpImplementation = static_cast<Implementation<SymbolTypes...> const *>(this);

                return crtpImplementation->template bind<T>();
            }

            template <typename T>
            typename T::value get() const {
                auto crtpImplementation = static_cast<Implementation<SymbolTypes...> const *>(this);

                return crtpImplementation->template get<T>();
            }
    };

}



namespace mosure::inversify {

    template <typename... SymbolTypes>
    class Container;

    template <typename... SymbolTypes>
    struct Context {
        const inversify::IContainer<Container, SymbolTypes...>& container;
    };

}

// #include <mosure/factory.hpp>


#include <functional>

// #include <mosure/context.hpp>



namespace mosure::inversify {

    template <typename T, typename... SymbolTypes>
    using Factory = std::function<T(const inversify::Context<SymbolTypes...>&)>;

}

// #include <mosure/resolver.hpp>


#include <atomic>
#include <memory>
#include <tuple>
#include <type_traits>

// #include <mosure/context.hpp>

// #include <mosure/factory.hpp>

// #include <mosure/injectable.hpp>


#include <tuple>
#include <type_traits>

// #include <mosure/context.hpp>

// #include <mosure/meta.hpp>


#include <type_traits>
#include <utility>

// #include <mosure/symbol.hpp>



namespace mosure::inversify::meta {

    template<typename T, typename... Types>
    inline constexpr bool contains_v = std::disjunction_v<
        std::is_same<T, Types>...
    >;

    template <typename... Args>
    struct is_empty : std::false_type { };

    template<>
    struct is_empty<> : std::true_type { };

    template <typename... Args>
    inline constexpr bool is_empty_v = is_empty<Args...>::value;

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type { };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type { };

    template <typename T>
    using value_t = typename T::value;

    template <typename ...Types>
    inline constexpr bool valid_symbol_types_v = std::conjunction_v<
        meta::is_specialization<Types, inversify::Symbol>...
    > || std::conjunction_v<
        std::is_base_of<inversify::Symbol<typename Types::value>, Types>...
    >;

}



namespace mosure::inversify {

    template <typename... Dependencies>
    struct Inject {
        static_assert(
            meta::valid_symbol_types_v<Dependencies...>,
            "inversify::Injectable dependencies must be of type inversify::Symbol"
        );

        template <typename... SymbolTypes>
        inline static auto resolve(const inversify::Context<SymbolTypes...>& context) {
            return std::make_tuple(context.container.template get<Dependencies>()...);
        }
    };

    template <typename Implementation, typename Inject = inversify::Inject<>>
    struct Injectable : Inject { };

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

    template <typename T, typename... SymbolTypes>
    class Resolver {
        public:
            inline virtual T resolve(const inversify::Context<SymbolTypes...>&) = 0;
    };

    template <typename T, typename... SymbolTypes>
    using ResolverPtr = std::shared_ptr<Resolver<T, SymbolTypes...>>;

    template <typename T, typename... SymbolTypes>
    class ConstantResolver : public Resolver<T, SymbolTypes...> {
        public:
            explicit ConstantResolver(T value) : value_(value) { }

            inline T resolve(const inversify::Context<SymbolTypes...>&) override {
                return value_;
            }

        private:
            T value_;
    };

    template <typename T, typename... SymbolTypes>
    class DynamicResolver : public Resolver<T, SymbolTypes...> {
        public:
            explicit DynamicResolver(inversify::Factory<T, SymbolTypes...> factory) : factory_(factory) { }

            inline T resolve(const inversify::Context<SymbolTypes...>& context) override {
                return factory_(context);
            }

        private:
            inversify::Factory<T, SymbolTypes...> factory_;
    };

    template <typename... SymbolTypes>
    class AutoResolver;

    template <typename T, typename U, typename... SymbolTypes>
    class AutoResolver<T, U, SymbolTypes...> : public Resolver<T, SymbolTypes...> {
        public:
            inline T resolve(const inversify::Context<SymbolTypes...>& context) override {
                return std::make_from_tuple<U>(
                    inversify::Injectable<U>::template resolve<SymbolTypes...>(context)
                );
            }
    };

    // unique_ptr specialization
    template <
        typename T,
        typename U,
        typename... SymbolTypes
    >
    class AutoResolver<std::unique_ptr<T>, U, SymbolTypes...> : public Resolver<std::unique_ptr<T>, SymbolTypes...> {
        public:
            inline std::unique_ptr<T> resolve(const inversify::Context<SymbolTypes...>& context) override {
                auto expansion = [&context](auto&& ... deps){
                    return std::make_unique<U>(deps...);
                };

                return std::apply(
                    expansion,
                    std::move(
                        inversify::Injectable<U>::template resolve<SymbolTypes...>(context)
                    )
                );
            }
    };

    // shared_ptr specialization
    template <
        typename T,
        typename U,
        typename... SymbolTypes
    >
    class AutoResolver<std::shared_ptr<T>, U, SymbolTypes...> : public Resolver<std::shared_ptr<T>, SymbolTypes...> {
        public:
            inline std::shared_ptr<T> resolve(const inversify::Context<SymbolTypes...>& context) override {
                auto expansion = [&context](auto&& ... deps){
                    return std::make_shared<U>(deps...);
                };

                return std::apply(
                    expansion,
                    std::move(
                        inversify::Injectable<U>::template resolve<SymbolTypes...>(context)
                    )
                );
            }
    };

    template <typename T, typename... SymbolTypes>
    class CachedResolver : public Resolver<T, SymbolTypes...> {
        static_assert(
            std::is_copy_constructible_v<T>,
            "inversify::CachedResolver requires a copy constructor. Are you caching a unique_ptr?"
        );

        public:
            explicit CachedResolver(ResolverPtr<T, SymbolTypes...> parent) : parent_(parent) { }

            inline T resolve(const inversify::Context<SymbolTypes...>& context) override {
                if (!hasCached_) {
                    hasCached_ = true;
                    cached_ = parent_->resolve(context);
                }

                return cached_;
            }

        private:
            T cached_;
            std::atomic<bool> hasCached_ { false };
            ResolverPtr<T, SymbolTypes...> parent_;
    };

}



namespace mosure::inversify {

    template <typename T, typename... SymbolTypes>
    class BindingScope {
        public:
            void inSingletonScope() {
                resolver_ = std::make_shared<inversify::CachedResolver<T, SymbolTypes...>>(resolver_);
            }

        protected:
            inversify::ResolverPtr<T, SymbolTypes...> resolver_;
    };

    template <typename T, typename... SymbolTypes>
    class BindingTo : public BindingScope<T, SymbolTypes...> {
        public:
            void toConstantValue(T&& value) {
                this->resolver_ = std::make_shared<inversify::ConstantResolver<T, SymbolTypes...>>(value);
            }

            BindingScope<T, SymbolTypes...>& toDynamicValue(inversify::Factory<T, SymbolTypes...> factory) {
                this->resolver_ = std::make_shared<inversify::DynamicResolver<T, SymbolTypes...>>(factory);

                return *this;
            }

            template <typename U>
            BindingScope<T, SymbolTypes...>& to() {
                this->resolver_ = std::make_shared<inversify::AutoResolver<T, U, SymbolTypes...>>();

                return *this;
            }
    };

    template <typename T, typename... SymbolTypes>
    class Binding : public BindingTo<typename T::value, SymbolTypes...> {
        public:
            inline typename T::value resolve(const Context<SymbolTypes...>& context) const {
                if (!this->resolver_) {
                    throw inversify::exceptions::ResolutionException("inversify::Resolver not found. Malformed binding.");
                }

                return this->resolver_->resolve(context);
            }
    };

}

// #include <mosure/container.hpp>


#include <tuple>

// #include <mosure/binding.hpp>

// #include <mosure/context.hpp>

// #include <mosure/interfaces/icontainer.hpp>

// #include <mosure/meta.hpp>



namespace mosure::inversify {

    using KeyType = int;

    template <typename... SymbolTypes>
    class Container
        :
        public inversify::IContainer<Container, SymbolTypes...> 
    {
        public:
            static_assert(
                meta::valid_symbol_types_v<SymbolTypes...>,
                "inversify::Container symbols must be of type inversify::Symbol"
            );

            static_assert(
                !meta::is_empty_v<SymbolTypes...>,
                "inversify::Container must register at least one symbol"
            );

            using BindingMap = std::tuple<
                inversify::Binding<
                    SymbolTypes,
                    SymbolTypes...
                >...
            >;

            template <typename T>
            inline inversify::BindingTo<typename T::value, SymbolTypes...>& bind() {
                static_assert(
                    meta::contains_v<T, SymbolTypes...>,
                    "inversify::Container symbol not registered"
                );

                return std::get<
                    inversify::Binding<T, SymbolTypes...>
                >(bindings_);
            }

            template <typename T>
            inline typename T::value get() const {
                static_assert(
                    meta::contains_v<T, SymbolTypes...>,
                    "inversify::Container symbol not registered"
                );

                return std::get<
                    inversify::Binding<T, SymbolTypes...>
                >(bindings_).resolve(context_);
            }

        private:
            BindingMap bindings_ {};
            inversify::Context<SymbolTypes...> context_ { *this };
    };

}

// #include <mosure/context.hpp>

// #include <mosure/factory.hpp>

// #include <mosure/injectable.hpp>

// #include <mosure/meta.hpp>

// #include <mosure/resolver.hpp>

// #include <mosure/symbol.hpp>


// #include <mosure/exceptions/resolution.hpp>



/**
    @brief namespace for Mitchell Mosure - InversifyCpp
    @see https://github.com/mosure
    @since version 1.0.0
*/
namespace mosure::inversify { }
