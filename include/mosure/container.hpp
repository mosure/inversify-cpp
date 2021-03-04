#pragma once

#include <mosure/binding.hpp>


namespace mosure::inversify {

    template <typename Symbol>
    struct BindingLookup {
        inline static inversify::Binding<typename Symbol::value> binding {};
    };

    template <typename T>
    inline static inversify::BindingTo<typename T::value>& bind() {
        return BindingLookup<T>::binding;
    }

    template <typename T>
    inline static typename T::value get() {
        return BindingLookup<T>::binding.resolve();
    }

}
