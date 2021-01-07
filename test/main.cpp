#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

struct IFizz {
    virtual ~IFizz() = default;

    virtual void buzz() = 0;
};

using IFizzPtr = std::shared_ptr<IFizz>;

struct Fizz : IFizz {
    Fizz(int foo, double bar)
        :
        foo_(foo),
        bar_(bar)
    { }

    void buzz() override {
        std::cout << "Fizz::buzz() - foo: " << foo_ << " - bar: " << bar_ << " - counter: " << ++counter_ << std::endl;
    }

    int foo_;
    int bar_;

    int counter_ { 0 };
};

namespace types {
    inversify::Symbol foo { "Foo" };
    inversify::Symbol bar { "Bar" };
    inversify::Symbol fizz { "Fizz" };
    inversify::Symbol fizzFactory { "FizzFactory" };
}

int main() {
    try {
        inversify::Container container {};

        container.bind<int>(types::foo)->toConstantValue(10);
        container.bind<double>(types::bar)->toConstantValue(1.618);

        container.bind<IFizzPtr>(types::fizz)->toDynamicValue(
            [](const inversify::Context& ctx) {
                auto foo = ctx.container.get<int>(types::foo);
                auto bar = ctx.container.get<double>(types::bar);

                auto fizz = std::make_shared<Fizz>(foo, bar);

                return fizz;
            }
        )->inSingletonScope();

        container.bind<std::function<IFizzPtr()>>(types::fizzFactory)->toDynamicValue(
            [](const inversify::Context& ctx) {
                return [&]() {
                    auto foo = ctx.container.get<int>(types::foo);
                    auto bar = ctx.container.get<double>(types::bar);

                    auto fizz = std::make_shared<Fizz>(foo, bar);

                    return fizz;
                };
            }
        )->inSingletonScope();

        auto fizz = container.get<IFizzPtr>(types::fizz);
        fizz->buzz();

        auto fizz2 = container.get<IFizzPtr>(types::fizz);
        fizz2->buzz();

        auto fizzFactory = container.get<std::function<IFizzPtr()>>(types::fizzFactory);

        auto anotherFizz = fizzFactory();
        anotherFizz->buzz();

        auto anotherFizz2 = fizzFactory();
        anotherFizz2->buzz();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
