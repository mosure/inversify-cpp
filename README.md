# InversifyCpp
C++ inversion of control and dependency injection container library.

## Features
- Compile time registration
- Compile time binding
- Compile time resolution

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
}

using IFizzPtr = std::unique_ptr<IFizz>;

```

```cpp

namespace types {
    inversify::symbol foo {"Foo"};
    inversify::symbol bar {"Bar"};
    inversify::symbol fizz {"Fizz"};
    inversify::symbol fizzPtr {"FizzPtr"};
}

```


#### Declare Classes and Dependencies

```cpp

struct Fizz : IFizz {
    Fizz(int foo, double bar);

    void buzz() override;
};

inversify::injectable<Fizz>(types::foo, types::bar);

```


#### Creating and Configuring a Container

```cpp

inversify::container container {};

container.bind<int>(types::foo).toConstantValue(10);
container.bind<double>(types::bar).toDynamicValue(
    [](inversify::context) {
        return 1.618;
    }
);

container.bind<IFizz>(types::fizz).to(Fizz).inSingletonScope();
container.bind<IFizzPtr>(types::fizzPtr).toPointer(Fizz);

```


#### Resolving Dependencies

```cpp

auto fizz = container.get<IFizz>(types::fizz);
fizz.buzz();

```
