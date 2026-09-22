#ifndef CHIP8QTEMULATOR_CHIP8_H
#define CHIP8QTEMULATOR_CHIP8_H

#include <array>
#include <cstdint>

namespace CHIP8CONF {
    constexpr  uint16_t START_PROGRAM_ADDRESS = 0x200;
    constexpr  uint16_t MEMORY_SIZE = 4096;
    constexpr  uint8_t REGISTER_SIZE = 16;
    constexpr  uint8_t STACK_SIZE = 16;
    constexpr  uint8_t FLAG_REGISTER_INDEX = 15;
    constexpr  uint8_t DISPLAY_WIDTH = 64;
    constexpr  uint8_t DISPLAY_HEIGHT = 32;

    constexpr uint16_t FONT_START_ADDRESS = 0x0000;
    constexpr uint8_t  FONT_CHAR_SIZE = 5;
    constexpr std::array<uint8_t, 16 * FONT_CHAR_SIZE> FONTSET = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
}


class Chip8 {
    private:
        uint8_t memory[CHIP8CONF::MEMORY_SIZE]{};
        uint8_t VX[CHIP8CONF::REGISTER_SIZE]{};             // General Purpose Register
        uint16_t I = 0;                                      // Address index
        uint8_t ST = 0;                                     // Sound Timer
        uint8_t DT = 0;                                     // Delay Timer
        uint8_t SP = 0;                                     // Stack Pointer
        uint16_t PC = CHIP8CONF::START_PROGRAM_ADDRESS;     // Program Counter
        uint16_t stack[CHIP8CONF::STACK_SIZE]{};            // Stack
        void execute(uint16_t opcode);
        
    public:
        uint8_t KB[CHIP8CONF::REGISTER_SIZE]{};                               // Keyboard commands
        uint8_t GB[CHIP8CONF::DISPLAY_WIDTH][CHIP8CONF::DISPLAY_HEIGHT]{};    // Graphics Buffer

        Chip8();
};

#endif //CHIP8QTEMULATOR_CHIP8_H
