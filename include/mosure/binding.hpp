#pragma once

#include <functional>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/factory.hpp>
#include <mosure/resolver.hpp>
#include <mosure/exceptions/resolution.hpp>


namespace mosure::inversify {

template <typename T, typename... SymbolTypes>
class BindingScope {
public:
    void inSingletonScope() {
        resolver_ = std::make_shared<inversify::CachedResolver<T, SymbolTypes...>>(resolver_);
    }

#ifdef INVERSIFY_BINDING_INSPECTION
    auto getResolver() const {
        return resolver_;
    }
#endif

protected:
    inversify::ResolverPtr<T, SymbolTypes...> resolver_;
};

template <typename T, typename... SymbolTypes>
class BindingTo
    : public BindingScope<T, SymbolTypes...>
{
public:
    void toConstantValue(T&& value) {
        this->resolver_ = std::make_shared<inversify::ConstantResolver<T, SymbolTypes...>>(value);
    }

    BindingScope<T, SymbolTypes...>& toDynamicValue(inversify::Factory<T, SymbolTypes...>&& factory) {
        this->resolver_ = std::make_shared<inversify::DynamicResolver<T, SymbolTypes...>>(factory);

        return *this;
    }

    template <typename U>
    BindingScope<T, SymbolTypes...>& to() {
        this->resolver_ = std::make_shared<inversify::AutoResolver<T, U, SymbolTypes...>>();

        return *this;
    }
};

template <typename T, typename... SymbolTypes>
class Binding
    : public BindingTo<typename T::value, SymbolTypes...>
{
public:
    inline typename T::value resolve(const inversify::Context<SymbolTypes...>& context) {
        if (!this->resolver_) {
            throw inversify::exceptions::ResolutionException("inversify::Resolver not found. Malformed binding.");
        }

        return this->resolver_->resolve(context);
    }
};

}
