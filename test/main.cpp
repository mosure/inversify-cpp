#include <iostream>

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

struct IFizz {
    virtual ~IFizz() = default;

    virtual void buzz() = 0;
};

using IFizzUniquePtr = std::unique_ptr<IFizz>;
using IFizzSharedPtr = std::shared_ptr<IFizz>;

namespace types {
    inversify::Symbol foo { "Foo" };
    inversify::Symbol bar { "Bar" };
    inversify::Symbol fizz { "Fizz" };
    inversify::Symbol fizzFactory { "FizzFactory" };
    inversify::Symbol autoFizz { "AutoFizz" };
    inversify::Symbol autoFizzUnique { "AutoFizzUnique" };
    inversify::Symbol autoFizzShared { "AutoFizzShared" };
}

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

static auto fizz = inversify::Injectable<Fizz>::inject(
    inversify::Inject<int>(types::foo),
    inversify::Inject<double>(types::bar)
);


int main() {
    try {
        inversify::Container container {};

        container.bind<int>(types::foo)->toConstantValue(10);
        container.bind<double>(types::bar)->toConstantValue(1.618);

        container.bind<IFizzSharedPtr>(types::fizz)->toDynamicValue(
            [](const inversify::Context& ctx) {
                auto foo = ctx.container.get<int>(types::foo);
                auto bar = ctx.container.get<double>(types::bar);

                auto fizz = std::make_shared<Fizz>(foo, bar);

                return fizz;
            }
        )->inSingletonScope();

        container.bind<std::function<IFizzUniquePtr()>>(types::fizzFactory)->toDynamicValue(
            [](const inversify::Context& ctx) {
                return [&]() {
                    auto foo = ctx.container.get<int>(types::foo);
                    auto bar = ctx.container.get<double>(types::bar);

                    auto fizz = std::make_unique<Fizz>(foo, bar);

                    return fizz;
                };
            }
        );

        container.bind<Fizz>(types::autoFizz)->to<Fizz>();
        container.bind<IFizzUniquePtr>(types::autoFizzUnique)->to<Fizz>();
        container.bind<IFizzSharedPtr>(types::autoFizzShared)->to<Fizz>()->inSingletonScope();

        auto fizz = container.get<IFizzSharedPtr>(types::fizz);
        fizz->buzz();

        auto fizz2 = container.get<IFizzSharedPtr>(types::fizz);
        fizz2->buzz();

        auto fizzFactory = container.get<std::function<IFizzUniquePtr()>>(types::fizzFactory);

        auto anotherFizz = fizzFactory();
        anotherFizz->buzz();

        auto anotherFizz2 = fizzFactory();
        anotherFizz2->buzz();

        auto autoFizz = container.get<Fizz>(types::autoFizz);
        autoFizz.buzz();

        auto autoFizzUnique = container.get<IFizzUniquePtr>(types::autoFizzUnique);
        autoFizzUnique->buzz();

        auto autoFizzShared = container.get<IFizzSharedPtr>(types::autoFizzShared);
        autoFizzShared->buzz();

        auto autoFizzShared2 = container.get<IFizzSharedPtr>(types::autoFizzShared);
        autoFizzShared2->buzz();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
