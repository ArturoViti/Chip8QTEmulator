#ifndef CHIP8QTEMULATOR_SEGMENTATIONFAULTEXCEPTION_H
#define CHIP8QTEMULATOR_SEGMENTATIONFAULTEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class SegmentationFaultException : public std::exception {
    public:
        explicit SegmentationFaultException(std::string address) : address(std::move(address)) { }
        [[nodiscard]] const char* what() const noexcept override { return address.c_str(); }

    private:
        std::string address;
};

#endif //CHIP8QTEMULATOR_SEGMENTATIONFAULTEXCEPTION_H
