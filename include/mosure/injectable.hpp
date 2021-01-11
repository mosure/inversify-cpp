#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

#include <mosure/factory.hpp>
#include <mosure/meta.hpp>
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
                    throw inversify::exceptions::InjectableException("inversify::Injectable duplicate injectable registration.");
                }
                Injectable::registered = true;

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
            inline static bool registered { false };

            template <typename Dependency>
            inline static typename Dependency::value resolve_dependency(const inversify::Context& context, Dependency dep) {
                auto symbol = static_cast<InjectBase>(dep).symbol;

                return context.container.get<typename Dependency::value>(symbol);
            }
    };

}
