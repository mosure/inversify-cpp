#pragma once

#include <any>
#include <unordered_map>

#include <mosure/binding.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    class Container {
        public:
            template <typename Interface>
            void bind(inversify::Symbol type);

            template <typename Interface>
            void resolve(inversify::Symbol type);

        private:
            std::unordered_map<
                inversify::Symbol,
                std::any            // typecheck on inversify::Binding type
            > bindings_;
    };

}
