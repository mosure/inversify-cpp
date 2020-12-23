#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

struct IFizz {
    virtual ~IFizz() = default;

    virtual void buzz() = 0;
};

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
    inversify::Container container {};

    container.bind<int>(types::foo).toConstantValue(10);
    container.bind<double>(types::bar).toConstantValue(1.618);

    container.bind<IFizz>(types::fizz).toDynamicValue<Fizz>(
        [](inversify::Context ctx) {
            auto foo = ctx.container.resolve<int>(types::foo);
            auto bar = ctx.container.resolve<double>(types::bar);

            Fizz fizz { foo, bar };

            return fizz;
        }
    );
}
