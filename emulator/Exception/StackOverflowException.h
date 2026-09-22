#ifndef CHIP8QTEMULATOR_STACKOVERFLOWEXCEPTION_H
#define CHIP8QTEMULATOR_STACKOVERFLOWEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class StackOverflowException : public std::exception {
    public:
        explicit StackOverflowException(std::string address) : address(std::move(address)) { }
        [[nodiscard]] const char* what() const noexcept override { return address.c_str(); }

    private:
        std::string address;
};

#endif //CHIP8QTEMULATOR_STACKOVERFLOWEXCEPTION_H
