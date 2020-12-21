#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

class A { };
class B { };

namespace types {
    inversify::Symbol foo { "Foo" };
}

template<> inversify::Symbols
inversify::injectable<A>::dependencies = { types::foo };

int main() {
    for (const auto& symbol : inversify::injectable<A>::dependencies) {
        std::cout << symbol << std::endl;
    }
}
