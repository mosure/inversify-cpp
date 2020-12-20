#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

class A { };
class B { };

namespace types {
    inversify::symbol foo { "Foo" };
}

template<>
inversify::symbols inversify::injectable<A>::dependencies = { types::foo };

int main() {
    for (const auto& symbol : inversify::injectable<A>::dependencies) {
        std::cout << symbol << std::endl;
    }
}
