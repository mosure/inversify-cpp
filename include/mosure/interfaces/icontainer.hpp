#pragma once

#include <memory>

#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename T>
    class BindingTo;

    template <typename Implementation>
    class IContainer {
        public:
            template <typename T>
            inversify::BindingTo<T>& bind(const inversify::Symbol<T>& type) {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->bind(type);
            }

            template <typename T>
            T get(const inversify::Symbol<T>& type) const {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->get(type);
            }
    };

}
