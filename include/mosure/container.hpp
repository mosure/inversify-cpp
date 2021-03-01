#pragma once

#include <mosure/binding.hpp>
#include <mosure/context.hpp>
#include <mosure/interfaces/icontainer.hpp>


namespace mosure::inversify {

    template <typename Symbol>
    struct BindingLookup {
        inline static inversify::Binding<typename Symbol::value> binding {};
    };

    class Container : public inversify::IContainer<Container> {
        public:
            template <typename T>
            inversify::BindingTo<typename T::value>& bind() {
                return BindingLookup<T>::binding;
            }

            template <typename T>
            typename T::value get() const {
                return BindingLookup<T>::binding.resolve(context_);
            }

        private:
            inversify::Context context_ { *this };
    };

}
