#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/meta.hpp>
#include <mosure/symbol.hpp>


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
