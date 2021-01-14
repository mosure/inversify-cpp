#pragma once

#include <memory>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/injectable.hpp>
#include <mosure/exceptions/resolution.hpp>


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
                // TODO: add lock for multi-thread support
                if (!hasCached_) {
                    hasCached_ = true;
                    cached_ = parent_->resolve(context);
                }

                return cached_;
            }

        private:
            T cached_;
            bool hasCached_ { false };
            ResolverPtr<T> parent_;
    };

}
