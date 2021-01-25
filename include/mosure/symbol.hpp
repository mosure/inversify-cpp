#pragma once

#include <iostream>


namespace mosure::inversify {

    namespace {
        inline static int counter = 0;
    }

    template <typename Interface>
    struct Symbol {
        Symbol() : id(counter++) { }

        const int id;
        using value = Interface;
    };

}
