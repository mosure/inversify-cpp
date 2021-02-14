#pragma once

#include <stdexcept>


namespace mosure::inversify::exceptions {

    struct SymbolException : public std::runtime_error {
        explicit SymbolException() : std::runtime_error("inversify::Symbol not found.") { }
    };

}
