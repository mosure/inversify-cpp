# inversify-cpp

[![Test Ubuntu](https://github.com/mosure/inversify-cpp/workflows/ubuntu/badge.svg)](https://github.com/Mosure/inversify-cpp/actions?query=workflow%3Aubuntu)
[![Test MacOS](https://github.com/mosure/inversify-cpp/workflows/macos/badge.svg)](https://github.com/Mosure/inversify-cpp/actions?query=workflow%3Amacos)
[![Test Windows](https://github.com/mosure/inversify-cpp/workflows/windows/badge.svg)](https://github.com/Mosure/inversify-cpp/actions?query=workflow%3Awindows)
[![GitHub License](https://img.shields.io/github/license/mosure/inversify-cpp)](https://raw.githubusercontent.com/mosure/inversify-cpp/main/LICENSE)
[![GitHub Last Commit](https://img.shields.io/github/last-commit/mosure/inversify-cpp)](https://github.com/mosure/inversify-cpp)
[![GitHub Releases](https://img.shields.io/github/v/release/mosure/inversify-cpp?include_prereleases&sort=semver)](https://github.com/mosure/inversify-cpp/releases)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ed98bee84ee14c8eb6ad6a0f85b94ca1)](https://www.codacy.com/gh/Mosure/inversify-cpp/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mosure/inversify-cpp&amp;utm_campaign=Badge_Grade)
[![GitHub Downloads](https://img.shields.io/github/downloads/mosure/inversify-cpp/total)](https://github.com/mosure/inversify-cpp/releases)
[![GitHub Issues](https://img.shields.io/github/issues/mosure/inversify-cpp)](https://github.com/mosure/inversify-cpp/issues)
[![Average time to resolve an issue](http://isitmaintained.com/badge/resolution/mosure/inversify-cpp.svg)](http://isitmaintained.com/project/mosure/inversify-cpp "Average time to resolve an issue")

C++17 inversion of control and dependency injection container library.

## Containerless Version

See the [`containerless` branch](https://github.com/mosure/inversify-cpp/tree/containerless) for a static binding (containerless) version of the library.

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
    using foo = inversify::Symbol<int>;
    using bar = inversify::Symbol<double>;
    using fizz = inversify::Symbol<IFizzPtr>;
    using fizzFactory = inversify::Symbol<std::function<IFizzPtr()>>;

    using autoFizzUnique = inversify::Symbol<std::function<IFizzUniquePtr>>;
    using autoFizzShared = inversify::Symbol<std::function<IFizzSharedPtr>>;
}

```

> Note: symbols which hold the same interface type may do so via structs which inherit inversify::Symbol

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

template <>
struct inversify::Injectable<Fizz>
    : inversify::Inject<
        symbols::foo,
        symbols::bar
    >
{ };

```

#### Configure Bindings

```cpp

inversify::Container<
    symbols::foo,
    symbols::bar
> container;

```

##### Constant Values

Constant bindings are always singletons.

```cpp

container.bind<symbols::foo>().toConstantValue(10);
container.bind<symbols::bar>().toConstantValue(1.618);

```

##### Dynamic Bindings

Dynamic bindings are resolved when calling `container.get...`.

By default, dynamic bindings have resolution scope (e.g. each call to `container.get...` calls the factory).

Singleton scope dynamic bindings cache the first resolution of the binding.

```cpp

container.bind<symbols::fizz>().toDynamicValue(
    [](auto& ctx) {
        auto foo = ctx.container.template get<symbols::foo>();
        auto bar = ctx.container.template get<symbols::bar>();

        auto fizz = std::make_shared<Fizz>(foo, bar);

        return fizz;
    }
).inSingletonScope();

```

##### Factory Bindings

Dynamic bindings can be used to resolve factory functions.

```cpp

container.bind<symbols::fizzFactory>().toDynamicValue(
    [](auto& ctx) {
        return [&]() {
            auto foo = ctx.container.template get<symbols::foo>();
            auto bar = ctx.container.template get<symbols::bar>();

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

container.bind<symbols::autoFizzUnique>().to<Fizz>();
container.bind<symbols::autoFizzShared>().to<Fizz>().inSingletonScope();

```

#### Resolving Dependencies

```cpp

auto bar = container.get<symbols::bar>();

auto fizz = container.get<symbols::fizz>();
fizz->buzz();

auto fizzFactory = container.get<symbols::fizzFactory>();
auto anotherFizz = fizzFactory();
anotherFizz->buzz();


auto autoFizzUnique = container.get<symbols::autoFizzUnique>();
autoFizzUnique->buzz();

auto autoFizzShared = container.get<symbols::autoFizzShared>();
autoFizzShared->buzz();

```

### Running Tests

Use the following to run tests:

`bazel run test --enable_platform_specific_config`

> Note: run the example app in a similar way: `bazel run example/simple --enable_platform_specific_config`

## TODOS
*   More compile-time checks
*   Resolution scope

### Profiling

Run the following to generate a callgrind file:

*   `bazel run example/profiling --enable_platform_specific_config --compilation_mode=dbg -s`
*   `valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ./bazel-bin/example/profiling/profiling`

## Generating `single_include` Variant

Run `python ./third_party/amalgamate/amalgamate.py -c ./third_party/amalgamate/config.json -s ./` from the root of the repository.

## Thanks

*   [InversifyJS](https://inversify.io/) - API design and inspiration for the project.
