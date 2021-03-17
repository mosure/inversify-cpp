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
#include <type_traits>      // apply, conjunction_v, disjunction_v, false_type, is_copy_constructable, is_same, true_type

// #include <mosure/binding.hpp>


#include <functional>
#include <type_traits>

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

    template <
        typename T,
        typename... SymbolTypes
    >
    class BindingTo;

    template <
        template <typename...> class Implementation,
        typename... SymbolTypes
    >
    class IContainer {
        public:
            template <typename T>
            inversify::BindingTo<typename T::value, SymbolTypes...>& bind() {
                auto crtpImplementation = static_cast<Implementation<SymbolTypes...> *>(this);

                return crtpImplementation->template bind<T>();
            }

            template <typename T>
            typename T::value get() {
                auto crtpImplementation = static_cast<Implementation<SymbolTypes...> *>(this);

                return crtpImplementation->template get<T>();
            }
    };

}



namespace mosure::inversify {

    template <typename... SymbolTypes>
    class Container;

    template <typename... SymbolTypes>
    struct Context {
        inversify::IContainer<Container, SymbolTypes...>& container;
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


#include <memory>
#include <type_traits>
#include <utility>

// #include <mosure/symbol.hpp>



namespace mosure::inversify::meta {

    template<
        typename T,
        typename... Types
    >
    inline constexpr bool contains_v = std::disjunction_v<
        std::is_same<T, Types>...
    >;


    template <typename... Args>
    struct is_empty : std::false_type { };

    template<>
    struct is_empty<> : std::true_type { };

    template <typename... Args>
    inline constexpr bool is_empty_v = is_empty<Args...>::value;


    template <
        typename T,
        template <typename...> typename Template
    >
    struct is_specialization : std::false_type { };

    template <
        template <typename...> typename Template,
        typename... Args
    >
    struct is_specialization<Template<Args...>, Template> : std::true_type { };


    template <typename T>
    struct is_unique_ptr : std::false_type { };

    template <
        typename T,
        typename D
    >
    struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type { };

    template <typename T>
    struct is_shared_ptr : std::false_type { };

    template <typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : std::true_type { };

    template <typename T>
    struct is_smart_ptr : std::false_type { };

    template <
        typename T,
        typename D
    >
    struct is_smart_ptr<std::unique_ptr<T, D>> : std::true_type { };

    template <typename T>
    struct is_smart_ptr<std::shared_ptr<T>> : std::true_type { };


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
    struct Injectable: Inject { };

}

// #include <mosure/meta.hpp>



namespace mosure::inversify {

    template <
        typename T,
        typename U,
        typename... SymbolTypes
    >
    inline static inversify::Factory<T, SymbolTypes...> get_auto_resolver() {
        if constexpr (meta::is_unique_ptr<T>::value) {
            return [](const inversify::Context<SymbolTypes...>& context) {
                auto expansion = [](auto&& ... deps){
                    return std::make_unique<U>(deps...);
                };

                return std::apply(
                    expansion,
                    std::move(
                        inversify::Injectable<U>::resolve(context)
                    )
                );
            };
        } else if constexpr (meta::is_shared_ptr<T>::value) {
            return [](const inversify::Context<SymbolTypes...>& context) {
                auto expansion = [](auto&& ... deps){
                    return std::make_shared<U>(deps...);
                };

                return std::apply(
                    expansion,
                    std::move(
                        inversify::Injectable<U>::resolve(context)
                    )
                );
            };
        } else {
            return [](const inversify::Context<SymbolTypes...>& context) {
                return std::make_from_tuple<U>(
                    inversify::Injectable<U>::resolve(context)
                );
            };
        }
    }
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

// #include <mosure/container.hpp>


#include <tuple>

// #include <mosure/binding.hpp>

// #include <mosure/context.hpp>

// #include <mosure/interfaces/icontainer.hpp>

// #include <mosure/meta.hpp>



namespace mosure::inversify {

    template <typename... SymbolTypes>
    class Container
        : public inversify::IContainer<Container, SymbolTypes...> 
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
            inline typename T::value get() {
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
