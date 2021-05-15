#pragma once

#include <exception>

class WindowException : public std::exception {
public:
    WindowException(const char* message) : message(message) {};

    const char* what() const throw() {
        return message;
    }

private:
    const char* message;
};
