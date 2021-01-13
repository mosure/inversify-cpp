#pragma once

#include <memory>

#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename T>
    class BindingTo;

    template <typename T>
    using BindingToPtr = std::shared_ptr<BindingTo<T>>;

    template <typename Implementation>
    class IContainer {
        public:
            template <typename T>
            inversify::BindingToPtr<T> bind(const inversify::Symbol& type) {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->template bind<T>(type);
            }

            template <typename T>
            T get(const inversify::Symbol& type) const {
                auto crtpImplementation = static_cast<Implementation const *>(this);

                return crtpImplementation->template get<T>(type);
            }
    };

}
