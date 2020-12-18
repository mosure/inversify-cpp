#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

class A { };

inversify::symbol testSymbol = "testSymbol";
//inversify::injectable<A>(testSymbol);


int main() {
    // for (const auto& symbol : inversify::injectable_store<int>::value) {
    //     std::cout << symbol << std::endl;
    // }
}
