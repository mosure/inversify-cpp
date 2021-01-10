#pragma once

#include <stdexcept>
#include <string>


namespace mosure::inversify::exceptions {

    struct InjectableException : public std::runtime_error {
        InjectableException(std::string msg) : std::runtime_error(msg) { }
    };

}
