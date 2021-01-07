#pragma once


namespace mosure::inversify {

    class Container;

    struct Context {
        const inversify::Container& container;
    };

}
