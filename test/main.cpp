#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

class A { };
class B { };

using foo = inversify::symbol<void>;

inversify::injectable<A, int>::dependencies<int>;


int main() {
    for (const auto& symbol : inversify::injectable<A, int>::dependencies<int>::value) {
        std::cout << symbol << std::endl;
    }
}
