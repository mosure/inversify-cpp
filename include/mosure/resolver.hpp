#pragma once

#include <memory>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
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
            ConstantResolver(T value) : value_(value) { }

            T resolve(const inversify::Context&) override {
                return value_;
            }

        private:
            T value_;
    };

    template <typename T>
    class DynamicResolver : public Resolver<T> {
        public:
            DynamicResolver(inversify::Factory<T> factory) : factory_(factory) { }

            T resolve(const inversify::Context& context) override {
                return factory_(context);
            }

        private:
            inversify::Factory<T> factory_;
    };

    template <typename T>
    class CachedResolver : public Resolver<T> {
        public:
            CachedResolver(ResolverPtr<T> parent) : parent_(parent) { }

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
