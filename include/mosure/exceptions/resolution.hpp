#pragma once

#include <stdexcept>
#include <string>


namespace mosure::inversify::exceptions {

    struct ResolutionException : public std::runtime_error {
        ResolutionException() : std::runtime_error("inversify::Container resolution failed.") { }
        ResolutionException(std::string msg) : std::runtime_error(msg) { }
    };

}
