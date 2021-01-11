#pragma once

#include <mosure/inversify.hpp>


namespace inversify = mosure::inversify;

namespace types {
    inline const inversify::Symbol foo { "Foo" };
    inline const inversify::Symbol bar { "Bar" };
    inline const inversify::Symbol fizz { "Fizz" };
    inline const inversify::Symbol fizzFactory { "FizzFactory" };

    inline const inversify::Symbol autoFizz { "AutoFizz" };
    inline const inversify::Symbol autoFizzUnique { "AutoFizzUnique" };
    inline const inversify::Symbol autoFizzShared { "AutoFizzShared" };
}
