#pragma once

#include <mosure/interfaces/icontainer.hpp>


namespace mosure::inversify {

    class Container;

    struct Context {
        const inversify::IContainer<Container>& container;
    };

}
