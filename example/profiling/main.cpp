#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

using int_symbol = inversify::Symbol<int>;

struct Service {
    int val;

    Service() : val(0) { }
    Service(int val) : val(val) { }

    void foo() {
        std::cout << "hello: " << val << std::endl;
    }
};

template <>
struct inversify::Injectable<Service>
    : inversify::Inject<
        int_symbol
    >
{ };

using test_symbol = inversify::Symbol<Service>;


template <typename Container>
void to_profile(Container& container) {
    container.template get<test_symbol>().foo();
}

int main() {
    inversify::Container<
        int_symbol,
        test_symbol
    > container;

    container.bind<int_symbol>().toConstantValue(3);
    container.bind<test_symbol>().to<Service>();

    to_profile(container);
}
