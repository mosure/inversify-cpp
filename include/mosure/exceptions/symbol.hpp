#pragma once

#include <stdexcept>

#include <mosure/symbol.hpp>


namespace mosure::inversify::exceptions {

    struct SymbolException : public std::runtime_error {
        SymbolException(const inversify::Symbol& symbol) : std::runtime_error("inversify::Symbol not found: " + symbol) { }
    };

}
