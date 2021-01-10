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
    inversify::Symbol autoFizz { "AutoFizz" };
    inversify::Symbol autoFizzUnique { "AutoFizzUnique" };
    inversify::Symbol autoFizzShared { "AutoFizzShared" };
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

static auto fizz = inversify::Injectable<Fizz>::inject(
    inversify::Inject<int>(types::foo),
    inversify::Inject<double>(types::bar)
);

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

##### Automatic Bindings

Dependencies can be resolved automatically using an automatic binding. Injectables are a prerequisite to the type being constructed.

Automatic bindings can generate instances, unique_ptr's, and shared_ptr's of a class. The returned type is determined by the binding interface.

```cpp

container.bind<Fizz>(types::autoFizz)->to<Fizz>();
container.bind<IFizzUniquePtr>(types::autoFizzUnique)->to<Fizz>();
container.bind<IFizzSharedPtr>(types::autoFizzShared)->to<Fizz>()->inSingletonScope();

```


#### Resolving Dependencies

```cpp

auto bar = container.get<double>(types::bar);

auto fizz = container.get<IFizzPtr>(types::fizz);
fizz->buzz();

auto fizzFactory = container.get<std::function<IFizzPtr()>>(types::fizzFactory);
auto anotherFizz = fizzFactory();
anotherFizz->buzz();


auto autoFizz = container.get<Fizz>(types::autoFizz);
autoFizz.buzz();

auto autoFizzUnique = container.get<IFizzUniquePtr>(types::autoFizzUnique);
autoFizzUnique->buzz();

auto autoFizzShared = container.get<IFizzSharedPtr>(types::autoFizzShared);
autoFizzShared->buzz();

```


### Running Tests

Use the following to run tests:

`bazel run test`

> Note: depending on your compiler, `.bazelrc` may need to be modified to a supported compiler flag (e.g. `build --copt="/std:c++17"` vs `build --copt="-std=c++17"`).


## TODOS
- More compile-time checks
- Thread safety

## Generating `single_include` Variant

Run `python ./third_party/amalgamate/amalgamate.py -c ./third_party/amalgamate/config.json -s ./` from the root of the repository.
