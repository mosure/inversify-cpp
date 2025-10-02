#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <mosure/interfaces/icontainer.hpp>


namespace mosure::inversify {

class ResolutionScope {
    struct CacheEntry {
        virtual ~CacheEntry() = default;
    };

    template <typename T>
    struct CacheValue : CacheEntry {
        explicit CacheValue(T value)
            : value_(std::move(value))
        { }

        const T& value() const { return value_; }

    private:
        T value_;
    };

public:
    template <typename T>
    bool contains(const void* key) const {
        return cache_.find(key) != cache_.end();
    }

    template <typename T>
    T get(const void* key) const {
        return static_cast<CacheValue<T>*>(cache_.at(key).get())->value();
    }

    template <typename T>
    void set(const void* key, T value) {
        cache_[key] = std::make_unique<CacheValue<T>>(std::move(value));
    }

    void clear() { cache_.clear(); }

private:
    std::unordered_map<const void*, std::unique_ptr<CacheEntry>> cache_;
};

template <typename... SymbolTypes>
class Container;

template <typename... SymbolTypes>
struct Context {
    inversify::IContainer<Container, SymbolTypes...>& container;
    ResolutionScope* resolutionScope { nullptr };
};

}
