# InversifyCpp

[![Test Ubuntu](https://github.com/mosure/inversifycpp/workflows/ubuntu/badge.svg)](https://github.com/Mosure/InversifyCpp/actions?query=workflow%3Aubuntu)
[![Test MacOS](https://github.com/mosure/inversifycpp/workflows/macos/badge.svg)](https://github.com/Mosure/InversifyCpp/actions?query=workflow%3Amacos)
[![Test Windows](https://github.com/mosure/inversifycpp/workflows/windows/badge.svg)](https://github.com/Mosure/InversifyCpp/actions?query=workflow%3Awindows)
[![GitHub License](https://img.shields.io/github/license/mosure/inversifycpp)](https://raw.githubusercontent.com/mosure/inversifycpp/main/LICENSE)
[![GitHub Last Commit](https://img.shields.io/github/last-commit/mosure/inversifycpp)](https://github.com/mosure/inversifycpp)
[![GitHub Releases](https://img.shields.io/github/v/release/mosure/inversifycpp?include_prereleases&sort=semver)](https://github.com/mosure/inversifycpp/releases)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ed98bee84ee14c8eb6ad6a0f85b94ca1)](https://www.codacy.com/gh/Mosure/InversifyCpp/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Mosure/InversifyCpp&amp;utm_campaign=Badge_Grade)
[![GitHub Downloads](https://img.shields.io/github/downloads/mosure/inversifycpp/total)](https://github.com/mosure/inversifycpp/releases)
[![GitHub Issues](https://img.shields.io/github/issues/mosure/inversifycpp)](https://github.com/mosure/inversifycpp/issues)
[![Average time to resolve an issue](http://isitmaintained.com/badge/resolution/mosure/inversifycpp.svg)](http://isitmaintained.com/project/mosure/inversifycpp "Average time to resolve an issue")

C++17 inversion of control and dependency injection container library.

## Features
*   Constant, dynamic, and automatic resolvers
*   Singleton, resolution (TODO), and unique scopes

## Documentation

### Scope
Scope manages the uniqueness of a dependency.

Singleton scopes are cached after the first resolution and will be returned on subsequent `container.get...` calls.

Resolution scopes are cached throughout the duration of a single `container.get...` call. A dependency tree with duplicate dependencies will resolve each to the same cached value.

By default, the unique scope is used (except for constant values). The unique scope will resolve a unique dependency for each `container.get...` call.

## Integration

```cpp

#include <mosure/inversify.hpp>

// for convenience
namespace inversify = mosure::inversify;

```

### Examples

#### Declare Interfaces

```cpp

struct IFizz {
    virtual ~IFizz() = default;

    virtual void buzz() = 0;
};

using IFizzPtr = std::unique_ptr<IFizz>;

```

#### Declare Types

```cpp

namespace symbols {
    inline const inversify::Symbol foo { "Foo" };
    inline const inversify::Symbol bar { "Bar" };
    inline const inversify::Symbol fizz { "Fizz" };
    inline const inversify::Symbol fizzFactory { "FizzFactory" };

    inline const inversify::Symbol autoFizz { "AutoFizz" };
    inline const inversify::Symbol autoFizzUnique { "AutoFizzUnique" };
    inline const inversify::Symbol autoFizzShared { "AutoFizzShared" };
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
        std::cout << "Fizz::buzz() - foo: " << foo_
                  << " - bar: " << bar_
                  << " - counter: " << ++counter_
                  << std::endl;
    }

    int foo_;
    int bar_;

    int counter_ { 0 };
};

inline auto injectFizz = inversify::Injectable<Fizz>::inject(
    inversify::Inject<int>(symbols::foo),
    inversify::Inject<double>(symbols::bar)
);

```

#### Configure Bindings

```cpp

inversify::Container container {};

```

##### Constant Values

Constant bindings are always singletons.

```cpp

container.bind<int>(symbols::foo).toConstantValue(10);
container.bind<double>(symbols::bar).toConstantValue(1.618);

```

##### Dynamic Bindings

Dynamic bindings are resolved when calling `container.get...`.

By default, dynamic bindings have resolution scope (e.g. each call to `container.get...` calls the factory).

Singleton scope dynamic bindings cache the first resolution of the binding.

```cpp

container.bind<IFizzPtr>(symbols::fizz).toDynamicValue(
    [](const inversify::Context& ctx) {
        auto foo = ctx.container.get<int>(symbols::foo);
        auto bar = ctx.container.get<double>(symbols::bar);

        auto fizz = std::make_shared<Fizz>(foo, bar);

        return fizz;
    }
).inSingletonScope();

```

##### Factory Bindings

Dynamic bindings can be used to resolve factory functions.

```cpp

container.bind<std::function<IFizzPtr()>>(symbols::fizzFactory).toDynamicValue(
    [](const inversify::Context& ctx) {
        return [&]() {
            auto foo = ctx.container.get<int>(symbols::foo);
            auto bar = ctx.container.get<double>(symbols::bar);

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

container.bind<Fizz>(symbols::autoFizz).to<Fizz>();
container.bind<IFizzUniquePtr>(symbols::autoFizzUnique).to<Fizz>();
container.bind<IFizzSharedPtr>(symbols::autoFizzShared).to<Fizz>().inSingletonScope();

```

#### Resolving Dependencies

```cpp

auto bar = container.get<double>(symbols::bar);

auto fizz = container.get<IFizzPtr>(symbols::fizz);
fizz->buzz();

auto fizzFactory = container.get<std::function<IFizzPtr()>>(symbols::fizzFactory);
auto anotherFizz = fizzFactory();
anotherFizz->buzz();


auto autoFizz = container.get<Fizz>(symbols::autoFizz);
autoFizz.buzz();

auto autoFizzUnique = container.get<IFizzUniquePtr>(symbols::autoFizzUnique);
autoFizzUnique->buzz();

auto autoFizzShared = container.get<IFizzSharedPtr>(symbols::autoFizzShared);
autoFizzShared->buzz();

```

### Running Tests

Use the following to run tests:

`bazel run test --enable_platform_specific_config`

> Note: run the example app in a similar way: `bazel run example --enable_platform_specific_config`

## TODOS
*   More compile-time checks
*   Thread safety
*   Resolution scope

## Generating `single_include` Variant

Run `python ./third_party/amalgamate/amalgamate.py -c ./third_party/amalgamate/config.json -s ./` from the root of the repository.

## Thanks

*   [InversifyJS](https://inversify.io/) - API design and inspiration for the project.
