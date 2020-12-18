#pragma once


namespace mosure::inversify {

    template <class T = void>
    struct symbol {
        using value = T;
    };

}
