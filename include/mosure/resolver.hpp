#pragma once

#include <atomic>
#include <memory>
#include <tuple>
#include <type_traits>

#ifdef INVERSIFY_BINDING_INSPECTION
#include <string>
#include <typeinfo>
#include <vector>
#endif

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/injectable.hpp>
#include <mosure/exceptions/resolution.hpp>


namespace mosure::inversify {

#ifdef INVERSIFY_BINDING_INSPECTION
template <typename Symbol>
struct Dependency {
    using value = Symbol;

    operator bool() { return dependent; }

    bool dependent;
};

template <typename... SymbolTypes>
class DependencyTuple {
public:
    template <typename Symbol>
    auto get() {
        return std::get<Dependency<Symbol>>(values);
    }

    template <typename Symbol>
    void set(bool value) {
        std::get<Dependency<Symbol>>(values).dependent = value;
    }

    std::tuple<
        inversify::Dependency<
            SymbolTypes
        >...
    > values;
};
#endif


template <
    typename T,
    typename... SymbolTypes
>
class Resolver {
public:
    inline virtual T resolve(const inversify::Context<SymbolTypes...>&) = 0;

#ifdef INVERSIFY_BINDING_INSPECTION
    inline virtual std::string getResolverLabel() const = 0;
    inline virtual inversify::DependencyTuple<SymbolTypes...> getDependencies() const {
        return DependencyTuple<SymbolTypes...>{};
    }
#endif
};

template <
    typename T,
    typename... SymbolTypes
>
using ResolverPtr = std::shared_ptr<Resolver<T, SymbolTypes...>>;


template <
    typename T,
    typename... SymbolTypes
>
class ConstantResolver
    : public Resolver<T, SymbolTypes...> {
public:
    explicit ConstantResolver(T value) : value_(value) { }

    inline T resolve(const inversify::Context<SymbolTypes...>&) override {
        return value_;
    }

#ifdef INVERSIFY_BINDING_INSPECTION
    inline virtual std::string getResolverLabel() const override {
        return std::string("constant - ") + typeid(T).name();
    }
#endif

private:
    T value_;
};


template <
    typename T,
    typename... SymbolTypes
>
class DynamicResolver
    : public Resolver<T, SymbolTypes...> {
public:
    explicit DynamicResolver(inversify::Factory<T, SymbolTypes...> factory) : factory_(factory) { }

    inline T resolve(const inversify::Context<SymbolTypes...>& context) override {
        return factory_(context);
    }

#ifdef INVERSIFY_BINDING_INSPECTION
    inline virtual std::string getResolverLabel() const override {
        return std::string("dynamic - ") + typeid(T).name();
    }
#endif

private:
    inversify::Factory<T, SymbolTypes...> factory_;
};


template <typename... Args>
class AutoResolver;

template <
    typename T,
    typename U,
    typename... SymbolTypes
>
class AutoResolverBase
    : public Resolver<T, SymbolTypes...> {
public:
#ifdef INVERSIFY_BINDING_INSPECTION
    inline virtual std::string getResolverLabel() const override {
        return std::string("auto - ") + typeid(U).name();
    }

    inline virtual inversify::DependencyTuple<SymbolTypes...> getDependencies() const {
        auto unroll = [](auto&&... deps) {
            DependencyTuple<SymbolTypes...> dep_tuple;

            auto enable_dep = [&dep_tuple](auto dep) {
                dep_tuple.template set<decltype(dep)>(true);
            };
            (enable_dep(deps), ...);

            return dep_tuple;
        };

        return std::apply(unroll, inversify::Injectable<U>::dependencies);
    }
#endif
};


template <
    typename T,
    typename U,
    typename... SymbolTypes
>
class AutoResolver<T, U, SymbolTypes...>
    : public AutoResolverBase<T, U, SymbolTypes...> {
public:
    inline virtual T resolve(const inversify::Context<SymbolTypes...>& context) override {
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
class AutoResolver<std::unique_ptr<T>, U, SymbolTypes...>
    : public AutoResolverBase<std::unique_ptr<T>, U, SymbolTypes...> {
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
class AutoResolver<std::shared_ptr<T>, U, SymbolTypes...>
    : public AutoResolverBase<std::shared_ptr<T>, U, SymbolTypes...> {
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


template <
    typename T,
    typename... SymbolTypes
>
class CachedResolver
    : public Resolver<T, SymbolTypes...> {
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

#ifdef INVERSIFY_BINDING_INSPECTION
    inline virtual std::string getResolverLabel() const override {
        return std::string("cached - ") + parent_->getResolverLabel();
    }
#endif

private:
    T cached_;
    std::atomic<bool> hasCached_ { false };
    ResolverPtr<T, SymbolTypes...> parent_;
};

}
