#ifndef CHIP8QTEMULATOR_ILLEGALINSTRUCTIONEXCEPTION_H
#define CHIP8QTEMULATOR_ILLEGALINSTRUCTIONEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class IllegalInstructionException : public std::exception {
    public:
        explicit IllegalInstructionException(std::string opcode) : opcode(std::move(opcode)) { }
        [[nodiscard]] const char* what() const noexcept override { return opcode.c_str(); }

    private:
        std::string opcode;
};

#endif //CHIP8QTEMULATOR_ILLEGALINSTRUCTIONEXCEPTION_H
