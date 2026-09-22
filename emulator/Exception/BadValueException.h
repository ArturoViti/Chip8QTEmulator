#ifndef CHIP8QTEMULATOR_BADVALUEEXCEPTION_H
#define CHIP8QTEMULATOR_BADVALUEEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class BadValueException : public std::exception {
    public:
        explicit BadValueException(std::string value) : value(std::move(value)) { }
        [[nodiscard]] const char* what() const noexcept override { return value.c_str(); }

    private:
        std::string value;
};

#endif //CHIP8QTEMULATOR_BADVALUEEXCEPTION_H
