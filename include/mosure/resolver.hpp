#pragma once

#include <atomic>
#include <memory>
#include <tuple>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/injectable.hpp>
#include <mosure/exceptions/resolution.hpp>


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
