#pragma once

#include <stdexcept>
#include <string>


namespace mosure::inversify::exceptions {

    struct ResolutionException : public std::runtime_error {
        ResolutionException(std::string msg) : std::runtime_error(msg) { }
    };

}
