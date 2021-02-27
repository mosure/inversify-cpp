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
            inversify::BindingTo<typename T::value>& bind() {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->template bind<T>();
            }

            template <typename T>
            typename T::value get() const {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->template get<T>();
            }
    };

}
