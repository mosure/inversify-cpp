#pragma once

#include <exception>


namespace mosure::inversify::exceptions {

    class ResolutionException : public std::exception {
        virtual const char* what() const throw()
        {
            return "Container resolution failed.";
        }
    };

}
