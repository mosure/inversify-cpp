#pragma once

#include <type_traits>
#include <variant>


namespace mosure::inversify {

    template <typename Interface>
    struct Symbol {
        static_assert(
            !std::is_abstract<Interface>(),
            "inversify::Container cannot bind/get abstract class value (use a smart pointer instead)."
        );

        using value = Interface;
    };

}
