#pragma once

#include <exception>


namespace mosure::inversify::exceptions {

    class SymbolException : public std::exception {
        virtual const char* what() const throw()
        {
            return "Symbol not found.";
        }
    };

}
