#ifndef CHIP8QTEMULATOR_SEGMENTATIONFAULTEXCEPTION_H
#define CHIP8QTEMULATOR_SEGMENTATIONFAULTEXCEPTION_H

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

class SegmentationFaultException : public std::runtime_error {
    public:
        SegmentationFaultException(const std::uint32_t address, const std::uint16_t opcode, const std::uint16_t pc)
            : std::runtime_error(buildMessage(address, opcode, pc)), address(address), opcode(opcode), pc(pc) { }

        [[nodiscard]] std::uint32_t getAddress() const noexcept { return address; }
        [[nodiscard]] std::uint16_t getOpcode() const noexcept { return opcode; }
        [[nodiscard]] std::uint16_t getPC() const noexcept { return pc; }

    private:
        static std::string buildMessage( const std::uint32_t address, const std::uint16_t opcode, const std::uint16_t pc ) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0')
                << "Segmentation fault: access to address 0x" << std::setw(4) << address
                << " by opcode 0x" << std::setw(4) << opcode
                << " at PC 0x" << std::setw(4) << pc;
            return oss.str();
        }

        std::uint32_t address;
        std::uint16_t opcode;
        std::uint16_t pc;
};

#endif // CHIP8QTEMULATOR_SEGMENTATIONFAULTEXCEPTION_H