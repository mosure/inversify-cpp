#pragma once

#include <iostream>

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <mosure/factory.hpp>
#include <mosure/symbol.hpp>
#include <mosure/exceptions/injectable.hpp>


namespace mosure::inversify {

    struct InjectBase {
        InjectBase(inversify::Symbol symbol) : symbol(symbol) {  }

        inversify::Symbol symbol;
    };

    template <typename Interface>
    struct Inject : InjectBase {
        Inject(inversify::Symbol symbol) : InjectBase(symbol) {  }

        using value = typename Interface;
    };


    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type { };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type { };

    template <typename ...Types>
    inline constexpr bool valid_inject_types_v = std::conjunction_v<
        is_specialization<Types, Inject>...
    >;


    template <typename Implementation>
    class Injectable {

        public:
            inline static inversify::Factory<Implementation> factory;
            inline static inversify::Factory<std::unique_ptr<Implementation>> factory_unique;
            inline static inversify::Factory<std::shared_ptr<Implementation>> factory_shared;

            template <typename... Dependencies>
            inline static Injectable inject(Dependencies... dependencies) {
                static_assert(valid_inject_types_v<Dependencies...>, "inversify::Injectable dependencies must be of type inversify::Inject");

                if (Injectable::registered) {
                    throw inversify::exceptions::InjectableException();
                }
                Injectable::registered = true;

                factory = [
                    deps = std::make_tuple(dependencies...)
                ](const inversify::Context& context) mutable {
                    auto expansion = [&context](auto&& ... deps){
                        auto args = std::make_tuple(to_interface(context, deps)...);

                        return std::make_from_tuple<Implementation>(args);
                    };

                    return std::apply(expansion, std::move(deps));
                };

                factory_unique = [
                    deps = std::make_tuple(dependencies...)
                ](const inversify::Context& context) mutable {
                    auto expansion = [&context](auto&& ... deps){
                        auto args = std::make_tuple(to_interface(context, deps)...);

                        return std::apply([](auto&&... args) {
                            return std::make_unique<Implementation>(std::forward<decltype(args)>(args)...);
                        }, std::move(args));
                    };

                    return std::apply(expansion, std::move(deps));
                };

                factory_shared = [
                    deps = std::make_tuple(dependencies...)
                ](const inversify::Context& context) mutable {
                    auto expansion = [&context](auto&& ... deps){
                        auto args = std::make_tuple(to_interface(context, deps)...);

                        return std::apply([](auto&&... args) {
                            return std::make_shared<Implementation>(std::forward<decltype(args)>(args)...);
                        }, std::move(args));
                    };

                    return std::apply(expansion, std::move(deps));
                };

                return Injectable {};
            }

        private:
            inline static bool registered { false };

            template <typename Dependency>
            inline static typename Dependency::value to_interface(const inversify::Context& context, Dependency dep) {
                auto symbol = static_cast<InjectBase>(dep).symbol;

                std::cout << "AutoResolver - " << symbol << std::endl;

                return context.container.get<typename Dependency::value>(symbol);
            }
    };

}
