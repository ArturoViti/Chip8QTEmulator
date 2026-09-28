#ifndef CHIP8QTEMULATOR_BADVALUEEXCEPTION_H
#define CHIP8QTEMULATOR_BADVALUEEXCEPTION_H

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

class BadValueException : public std::runtime_error {
    public:
        BadValueException( const std::uint16_t value, const std::uint16_t opcode, const std::uint16_t pc )
            : std::runtime_error(buildMessage(value, opcode, pc)), value(value), opcode(opcode), pc(pc) { }

        [[nodiscard]] std::uint16_t getValue() const noexcept { return value; }
        [[nodiscard]] std::uint16_t getOpcode() const noexcept { return opcode; }
        [[nodiscard]] std::uint16_t getPC() const noexcept { return pc; }

    private:
        static std::string buildMessage( const std::uint16_t value, const std::uint16_t opcode, const std::uint16_t pc ) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0')
                << "Bad value 0x" << std::setw(2) << value
                << " in opcode 0x" << std::setw(4) << opcode
                << " at PC 0x" << std::setw(4) << pc;
            return oss.str();
        }

        std::uint16_t value;
        std::uint16_t opcode;
        std::uint16_t pc;
};

#endif // CHIP8QTEMULATOR_BADVALUEEXCEPTION_H