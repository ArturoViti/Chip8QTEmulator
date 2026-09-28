#ifndef CHIP8QTEMULATOR_STACKOVERFLOWEXCEPTION_H
#define CHIP8QTEMULATOR_STACKOVERFLOWEXCEPTION_H

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

class StackOverflowException : public std::runtime_error {
    public:
        StackOverflowException(const std::size_t sp, const std::size_t stackSize,
            const std::uint16_t opcode, const std::uint16_t pc
        ): std::runtime_error(buildMessage(sp, stackSize, opcode, pc)), sp(sp), stackSize(stackSize), opcode(opcode),
        pc(pc) { }

        [[nodiscard]] std::size_t getSP() const noexcept { return sp; }
        [[nodiscard]] std::size_t getStackSize() const noexcept { return stackSize; }
        [[nodiscard]] std::uint16_t getOpcode() const noexcept { return opcode; }
        [[nodiscard]] std::uint16_t getPC() const noexcept { return pc; }

    private:
        static std::string buildMessage( const std::size_t sp, const std::size_t stackSize,
            const std::uint16_t opcode, const std::uint16_t pc
        ) {
            std::ostringstream oss;
            oss << "Stack overflow: SP = " << sp << " (stack size " << stackSize << ")"
                << std::hex << std::uppercase << std::setfill('0')
                << ", call to 0x" << std::setw(3) << (opcode & 0x0FFF)
                << " by opcode 0x" << std::setw(4) << opcode
                << " at PC 0x" << std::setw(4) << pc;
            return oss.str();
        }

        std::size_t sp;
        std::size_t stackSize;
        std::uint16_t opcode;
        std::uint16_t pc;
};

#endif // CHIP8QTEMULATOR_STACKOVERFLOWEXCEPTION_H