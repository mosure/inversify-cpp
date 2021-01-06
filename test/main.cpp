#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

struct IFizz {
    virtual ~IFizz() = default;

    virtual void buzz() = 0;
};

using IFizzPtr = std::unique_ptr<IFizz>;

struct Fizz : IFizz {
    Fizz(int foo, double bar)
        :
        foo_(foo),
        bar_(bar)
    { }

    void buzz() override {
        std::cout << "Fizz::buzz() - foo: " << foo_ << " - bar: " << bar_ << std::endl;
    }

    int foo_;
    int bar_;
};

namespace types {
    inversify::Symbol foo { "Foo" };
    inversify::Symbol bar { "Bar" };
    inversify::Symbol fizz { "Fizz" };
}

int main() {
    try {
        std::cout << "InversifyCpp test starting..." << std::endl;

        inversify::Container container {};

        container.bind<int>(types::foo)->toConstantValue(10);
        container.bind<double>(types::bar)->toConstantValue(1.618);

        container.bind<IFizzPtr>(types::fizz)->toDynamicValue(
            [](inversify::Context ctx) {
                std::cout << "Dynamic binding called." << std::endl;

                auto foo = ctx.container.get<int>(types::foo);
                auto bar = ctx.container.get<double>(types::bar);

                auto fizz = std::make_unique<Fizz>(foo, bar);

                return fizz;
            }
        );

        auto bar = container.get<double>(types::bar);

        std::cout << "Constant Dependency resolved: " << bar << std::endl;

        auto fizz = container.get<IFizzPtr>(types::fizz);

        std::cout << "Dynamic Dependency resolved." << std::endl;

        fizz->buzz();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
