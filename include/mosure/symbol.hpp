#pragma once

#include <string>
#include <vector>


namespace mosure::inversify {

    template <typename T>
    struct symbol {
        using value = T;
    }

}
