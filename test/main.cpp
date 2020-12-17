#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

//constexpr inversify::symbol testSymbol = "testSymbol";

class A { };
class B { };
class C { };

int main() {
    inversify::injectable<int, char, bool>;

    for (const auto& symbol : inversify::injectable<A>::value) {
        std::cout << symbol << std::endl;
    }
}
