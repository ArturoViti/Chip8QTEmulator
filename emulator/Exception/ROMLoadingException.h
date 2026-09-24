#ifndef CHIP8QTEMULATOR_ROMLOADINGEXCEPTION_H
#define CHIP8QTEMULATOR_ROMLOADINGEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class ROMLoadingException : public std::exception {
    public:
        explicit ROMLoadingException(std::string address) : msg(std::move(address)) { }
        [[nodiscard]] const char* what() const noexcept override { return msg.c_str(); }

    private:
        std::string msg;
};

#endif //CHIP8QTEMULATOR_ROMLOADINGEXCEPTION_H
