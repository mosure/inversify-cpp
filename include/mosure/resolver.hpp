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

    template <typename T>
    class Resolver {
        public:
            inline virtual T resolve(const inversify::Context&) = 0;
    };

    template <typename T>
    using ResolverPtr = std::shared_ptr<Resolver<T>>;

    template <typename T>
    class ConstantResolver : public Resolver<T> {
        public:
            explicit ConstantResolver(T value) : value_(value) { }

            inline T resolve(const inversify::Context&) override {
                return value_;
            }

        private:
            T value_;
    };

    template <typename T>
    class DynamicResolver : public Resolver<T> {
        public:
            explicit DynamicResolver(inversify::Factory<T> factory) : factory_(factory) { }

            inline T resolve(const inversify::Context& context) override {
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
            inline T resolve(const inversify::Context& context) override {
                return std::make_from_tuple<U>(inversify::Injectable<U>::resolve(context));
            }
    };

    // unique_ptr specialization
    template <
        typename T,
        typename U
    >
    class AutoResolver<std::unique_ptr<T>, U> : public Resolver<std::unique_ptr<T>> {
        public:
            inline std::unique_ptr<T> resolve(const inversify::Context& context) override {
                auto expansion = [&context](auto&& ... deps){
                    return std::make_unique<U>(deps...);
                };

                return std::apply(expansion, std::move(inversify::Injectable<U>::resolve(context)));
            }
    };

    // shared_ptr specialization
    template <
        typename T,
        typename U
    >
    class AutoResolver<std::shared_ptr<T>, U> : public Resolver<std::shared_ptr<T>> {
        public:
            inline std::shared_ptr<T> resolve(const inversify::Context& context) override {
                auto expansion = [&context](auto&& ... deps){
                    return std::make_shared<U>(deps...);
                };

                return std::apply(expansion, std::move(inversify::Injectable<U>::resolve(context)));
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

            inline T resolve(const inversify::Context& context) override {
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
