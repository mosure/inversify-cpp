# InversifyCpp
C++ inversion of control and dependency injection container library.

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

```cpp

namespace types {
    inversify::Symbol foo { "Foo" };
    inversify::Symbol bar { "Bar" };
    inversify::Symbol fizz { "Fizz" };
}

```


#### Declare Classes and Dependencies

```cpp

struct IFizz {
    virtual ~IFizz() = default;

    virtual void buzz() = 0;
};

using IFizzPtr = std::shared_ptr<IFizz>;

```


#### Creating and Configuring a Container

```cpp

inversify::Container container {};

container.bind<int>(types::foo)->toConstantValue(10);
container.bind<double>(types::bar)->toConstantValue(1.618);

container.bind<IFizzPtr>(types::fizz)->toDynamicValue(
    [](inversify::Context ctx) {
        auto foo = ctx.container.get<int>(types::foo);
        auto bar = ctx.container.get<double>(types::bar);

        auto fizz = std::make_shared<Fizz>(foo, bar);

        return fizz;
    }
)->inSingletonScope();

```


#### Resolving Dependencies

```cpp

auto bar = container.get<double>(types::bar);

auto fizz = container.get<IFizzPtr>(types::fizz);
fizz->buzz();

```
