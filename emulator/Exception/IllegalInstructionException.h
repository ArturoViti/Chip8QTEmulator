#ifndef CHIP8QTEMULATOR_ILLEGALINSTRUCTIONEXCEPTION_H
#define CHIP8QTEMULATOR_ILLEGALINSTRUCTIONEXCEPTION_H

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

class IllegalInstructionException : public std::runtime_error {
    public:
        IllegalInstructionException( const std::uint16_t opcode, const std::uint16_t pc )
            : std::runtime_error(buildMessage(opcode, pc)), opcode(opcode), pc(pc) { }

        [[nodiscard]] std::uint16_t getOpcode() const noexcept { return opcode; }
        [[nodiscard]] std::uint16_t getPC() const noexcept { return pc; }

    private:
        static std::string buildMessage( const std::uint16_t opcode, const std::uint16_t pc ) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0')
                << "Illegal instruction 0x" << std::setw(4) << opcode
                << " at PC 0x" << std::setw(4) << pc;
            return oss.str();
        }

        std::uint16_t opcode;
        std::uint16_t pc;
};

#endif // CHIP8QTEMULATOR_ILLEGALINSTRUCTIONEXCEPTION_H