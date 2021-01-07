# InversifyCpp
C++17 inversion of control and dependency injection container library.


## Features
- Constant, dynamic, and factory resolvers
- Singleton and per-resolution scopes


## Integration

```cpp

#include <mosure/inversify.hpp>

// for convenience
namespace inversify = mosure::inversify;

```


### Examples

#### Declare Interfaces

```cpp

#include <memory>


struct IFizz {
    virtual ~IFizz() = default;

    virtual void buzz() = 0;
};

using IFizzPtr = std::unique_ptr<IFizz>;

```

#### Declare Types

```cpp

namespace types {
    inversify::Symbol foo { "Foo" };
    inversify::Symbol bar { "Bar" };
    inversify::Symbol fizz { "Fizz" };
    inversify::Symbol fizzFactory { "FizzFactory" };
}

```


#### Declare Classes and Dependencies

```cpp

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

```


#### Configure Bindings

```cpp

inversify::Container container {};

```

##### Constant Values

Constant bindings are always singletons.

```cpp

container.bind<int>(types::foo)->toConstantValue(10);
container.bind<double>(types::bar)->toConstantValue(1.618);

```

##### Dynamic Bindings

Dynamic bindings are resolved when calling `container.get...`.

By default, dynamic bindings have resolution scope (e.g. each call to `container.get...` calls the factory).

Singleton scope dynamic bindings cache the first resolution of the binding.

```cpp

container.bind<IFizzPtr>(types::fizz)->toDynamicValue(
    [](const inversify::Context& ctx) {
        auto foo = ctx.container.get<int>(types::foo);
        auto bar = ctx.container.get<double>(types::bar);

        auto fizz = std::make_shared<Fizz>(foo, bar);

        return fizz;
    }
)->inSingletonScope();

```

##### Factory Bindings

Dynamic bindings can be used to resolve factory functions.

```cpp

container.bind<std::function<IFizzPtr()>>(types::fizzFactory)->toDynamicValue(
    [](const inversify::Context& ctx) {
        return [&]() {
            auto foo = ctx.container.get<int>(types::foo);
            auto bar = ctx.container.get<double>(types::bar);

            auto fizz = std::make_shared<Fizz>(foo, bar);

            return fizz;
        };
    }
);

```

#### Resolving Dependencies

```cpp

auto bar = container.get<double>(types::bar);

auto fizz = container.get<IFizzPtr>(types::fizz);
fizz->buzz();

auto fizzFactory = container.get<std::function<IFizzPtr()>>(types::fizzFactory);
auto anotherFizz = fizzFactory();
anotherFizz->buzz();

```


### Running Tests

Use the following to run tests:

`bazel run test`

> Note: depending on your compiler, `.bazelrc` may need to be modified to a supported compiler flag (e.g. `build --copt="/std:c++17"` vs `build --copt="-std=c++17"`).


## TODOS
- Automatic resolution of dependencies (generation of factory methods)
- More compile-time checks
