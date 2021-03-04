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
#include <type_traits>      // apply, conjunction_v, false_type, true_type

// #include <mosure/binding.hpp>


#include <functional>
#include <memory>

// #include <mosure/factory.hpp>


#include <functional>


namespace mosure::inversify {

    template <typename T>
    using Factory = std::function<T()>;

}

// #include <mosure/resolver.hpp>


#include <atomic>
#include <memory>
#include <tuple>
#include <type_traits>

// #include <mosure/factory.hpp>

// #include <mosure/injectable.hpp>


#include <tuple>
#include <type_traits>

// #include <mosure/meta.hpp>


#include <type_traits>
#include <utility>

// #include <mosure/symbol.hpp>


#include <type_traits>


namespace mosure::inversify {

    template <typename Interface>
    struct Symbol {
        static_assert(!std::is_abstract<Interface>(), "inversify::Container cannot bind/get abstract class value (use a smart pointer instead).");

        using value = Interface;
    };

}



namespace mosure::inversify::meta {

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type { };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type { };

    template <typename ...Types>
    inline constexpr bool valid_inject_types_v = std::conjunction_v<
        meta::is_specialization<Types, inversify::Symbol>...
    >;

    template <typename T>
    using value_t = typename T::value;

}

// #include <mosure/symbol.hpp>



namespace mosure::inversify {

    template <typename T>
    static typename T::value get();

    template <typename... Dependencies>
    struct Inject {
        static_assert(meta::valid_inject_types_v<Dependencies...>, "inversify::Injectable dependencies must be of type inversify::Symbol");

        inline static auto resolve() {
            return std::make_tuple(inversify::get<Dependencies>()...);
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

    template <typename T>
    class Resolver {
        public:
            inline virtual T resolve() = 0;
    };

    template <typename T>
    using ResolverPtr = std::shared_ptr<Resolver<T>>;

    template <typename T>
    class ConstantResolver : public Resolver<T> {
        public:
            explicit ConstantResolver(T value) : value_(value) { }

            inline T resolve() override {
                return value_;
            }

        private:
            T value_;
    };

    template <typename T>
    class DynamicResolver : public Resolver<T> {
        public:
            explicit DynamicResolver(inversify::Factory<T> factory) : factory_(factory) { }

            inline T resolve() override {
                return factory_();
            }

        private:
            inversify::Factory<T> factory_;
    };

    template <typename T, typename U, typename = void>
    class AutoResolver;

    template <typename T, typename U>
    class AutoResolver<T, U> : public Resolver<T> {
        public:
            inline T resolve() override {
                return std::make_from_tuple<U>(inversify::Injectable<U>::resolve());
            }
    };

    // unique_ptr specialization
    template <
        typename T,
        typename U
    >
    class AutoResolver<std::unique_ptr<T>, U> : public Resolver<std::unique_ptr<T>> {
        public:
            inline std::unique_ptr<T> resolve() override {
                auto expansion = [](auto&& ... deps){
                    return std::make_unique<U>(deps...);
                };

                return std::apply(expansion, std::move(inversify::Injectable<U>::resolve()));
            }
    };

    // shared_ptr specialization
    template <
        typename T,
        typename U
    >
    class AutoResolver<std::shared_ptr<T>, U> : public Resolver<std::shared_ptr<T>> {
        public:
            inline std::shared_ptr<T> resolve() override {
                auto expansion = [](auto&& ... deps){
                    return std::make_shared<U>(deps...);
                };

                return std::apply(expansion, std::move(inversify::Injectable<U>::resolve()));
            }
    };

    template <typename T>
    class CachedResolver : public Resolver<T> {
        static_assert(
            std::is_copy_constructible_v<T>,
            "inversify::CachedResolver requires a copy constructor. Are you caching a unique_ptr?"
        );

        public:
            explicit CachedResolver(ResolverPtr<T> parent) : parent_(parent) { }

            inline T resolve() override {
                if (!hasCached_) {
                    hasCached_ = true;
                    cached_ = parent_->resolve();
                }

                return cached_;
            }

        private:
            T cached_;
            std::atomic<bool> hasCached_ { false };
            ResolverPtr<T> parent_;
    };

}



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

// #include <mosure/container.hpp>


// #include <mosure/binding.hpp>



namespace mosure::inversify {

    template <typename Symbol>
    struct BindingLookup {
        inline static inversify::Binding<typename Symbol::value> binding {};
    };

    template <typename T>
    inline static inversify::BindingTo<typename T::value>& bind() {
        return BindingLookup<T>::binding;
    }

    template <typename T>
    inline static typename T::value get() {
        return BindingLookup<T>::binding.resolve();
    }

}

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
