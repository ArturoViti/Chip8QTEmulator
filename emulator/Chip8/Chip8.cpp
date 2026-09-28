#include <algorithm>
#include <iterator>
#include <random>
#include <fstream>

#include <Chip8/Chip8.h>
#include <Exception/IllegalInstructionException.h>
#include <Exception/SegmentationFaultException.h>
#include <Exception/StackOverflowException.h>

#include "Exception/BadValueException.h"
#include "Exception/ROMLoadingException.h"

Chip8::Chip8() {
    // Font must be in RAM at boot
    std::copy(
        CHIP8CONF::FONTSET.begin(),
        CHIP8CONF::FONTSET.end(),
        std::begin(memory) + CHIP8CONF::FONT_START_ADDRESS
    );
}

Chip8::~Chip8() { this->reset(); }

void Chip8::execute( const uint16_t opcode ) {
    /*
     * nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
     * n or nibble - A 4-bit value, the lowest 4 bits of the instruction
     * x - A 4-bit value, the lower 4 bits of the high byte of the instruction
     * y - A 4-bit value, the upper 4 bits of the low byte of the instruction
     * kk or byte - An 8-bit value, the lowest 8 bits of the instruction
     */
    // AND operation permit to set min value and max value.
    const uint16_t nnn = opcode & 0x0FFF;
    const uint8_t n = opcode & 0x000F;
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;
    const uint8_t kk = opcode & 0x00FF;

    // Get first 4-bit of MSByte
    switch (opcode & 0xF000)
    {
        // Mono-operand opcodes
        case 0x0000:
        {
            switch (opcode)
            {
                // 00E0 - CLS. Clear the display.
                case 0x00E0:
                {
                    for ( auto & i : this->GB )
                        for ( uint8_t & j : i)
                            j = 0x00;
                    break;
                }
                // 00EE - RET. Return from a subroutine.
                case 0x00EE:
                {
                    this->PC = this->stack[SP];
                    SP--;
                    break;
                }
                default:
                    throw IllegalInstructionException(opcode, this->PC);
            }
            break;
        }
        // 1nnn - JP addr. Jump to location nnn.
        case 0x1000:
        {
            if ( nnn >= CHIP8CONF::START_PROGRAM_ADDRESS && nnn <= CHIP8CONF::MEMORY_SIZE - 1 )
                this->PC = nnn;
            else
                throw SegmentationFaultException(nnn, opcode, this->PC);

            break;
        }
        // 2nnn - CALL addr. Call subroutine at nnn.
        case 0x2000:
        {
            // The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
            if ( nnn >= CHIP8CONF::START_PROGRAM_ADDRESS && nnn <= CHIP8CONF::MEMORY_SIZE - 1 )
            {
                if ( this->SP + 1 >= CHIP8CONF::STACK_SIZE )
                    throw StackOverflowException(SP, CHIP8CONF::STACK_SIZE, opcode, this->PC);

                this->SP++;
                this->stack[SP] = this->PC;
                this->PC = nnn;
            }
            else
                throw SegmentationFaultException(nnn, opcode, this->PC);

            break;
        }
        // 3xkk - SE Vx, byte. Skip next instruction if Vx = kk.
        case 0x3000:
        {
            if ( this->VX[x] == kk )
            {
                if ( this->PC + 2 > CHIP8CONF::MEMORY_SIZE )
                    throw SegmentationFaultException(this->PC + 2, opcode, this->PC);

                this->PC += 2;
            }
            break;
        }
        // 4xkk - SNE Vx, byte. Skip next instruction if Vx != kk.
        case 0x4000:
        {
            if ( this->VX[x] != kk )
            {
               if ( this->PC + 2 > CHIP8CONF::MEMORY_SIZE )
                   throw SegmentationFaultException(this->PC + 2, opcode, this->PC);

                this->PC += 2;
            }
            break;
        }
        // 5xy0 - SE Vx, Vy. Skip next instruction if Vx = Vy.
        case 0x5000:
        {
            if ( n != 0x00 )
                throw IllegalInstructionException(opcode, this->PC);

            if ( this->VX[x] == this->VX[y] )
            {
                if ( this->PC + 2 > CHIP8CONF::MEMORY_SIZE )
                    throw SegmentationFaultException(this->PC + 2, opcode, this->PC);

                this->PC += 2;
            }
            break;
        }
        // 6xkk - LD Vx, byte. Set Vx = kk.
        case 0x6000: { this->VX[x] = kk; break; }
        // 7xkk - ADD Vx, byte. Set Vx = Vx + kk.
        case 0x7000:
        {
            if ( x < 0 || x >= CHIP8CONF::REGISTER_SIZE )
                throw BadValueException(x, opcode, this->PC);
            if ( kk > 0xFF )
                throw BadValueException(kk, opcode, this->PC);

            this->VX[x] += kk;
            break;
        }
        case 0x8000:
        {
            switch ( n )
            {
                // 8xy0 - LD Vx, Vy. Set Vx = Vy.
                case 0x0: this->VX[x] = this->VX[y];  break;
                // 8xy1 - OR Vx, Vy. Set Vx = Vx OR Vy.
                case 0x1: this->VX[x] |= this->VX[y]; break;
                // 8xy2 - AND Vx, Vy. Set Vx = Vx AND Vy.
                case 0x2: this->VX[x] &= this->VX[y]; break;
                // 8xy3 - XOR Vx, Vy. Set Vx = Vx XOR Vy.
                case 0x3: this->VX[x] ^= this->VX[y]; break;
                // 8xy4 - ADD Vx, Vy. Set Vx = Vx + Vy, set VF = carry.
                case 0x4:
                {
                    const uint16_t sum = this->VX[x] + this->VX[y];
                    this->VX[CHIP8CONF::FLAG_REGISTER_INDEX] = sum > 0xFF ? 1 : 0;
                    this->VX[x] = sum & 0xFF;
                    break;
                }
                // 8xy5 - SUB Vx, Vy. Set Vx = Vx - Vy, set VF = NOT borrow.
                case 0x5:
                {
                    this->VX[CHIP8CONF::FLAG_REGISTER_INDEX] = this->VX[x] > this->VX[y] ? 1 : 0;
                    this->VX[x] -= this->VX[y];
                    break;
                }
                // 8xy6 - SHR Vx {, Vy}. Set Vx = Vx SHR 1.
                case 0x6:
                {
                    const uint8_t flag = this->VX[x] & 0b00000001;
                    this->VX[x] >>= 1;
                    this->VX[CHIP8CONF::FLAG_REGISTER_INDEX] = flag;
                    break;
                }
                // 8xy7 - SUBN Vx, Vy. Set Vx = Vy - Vx, set VF = NOT borrow.
                case 0x7:
                {
                    this->VX[CHIP8CONF::FLAG_REGISTER_INDEX] = this->VX[y] > this->VX[x] ? 1 : 0;
                    this->VX[x] = this->VX[y] - this->VX[x];
                    break;
                }
                // 8xyE - SHL Vx {, Vy} Set Vx = Vx SHL 1.
                case 0xE:
                {
                    const uint8_t flag = this->VX[x] & 0b10000000;
                    this->VX[x] <<= 1;
                    this->VX[CHIP8CONF::FLAG_REGISTER_INDEX] = flag;
                    break;
                }
                default:
                    throw IllegalInstructionException(opcode, this->PC);
            }
            break;
        }
        // 9xy0 - SNE Vx, Vy. Skip next instruction if Vx != Vy.
        case 0x9000:
        {
            if ( this->VX[x] != this->VX[y] )
            {
                if ( this->PC + 2 > CHIP8CONF::MEMORY_SIZE )
                    throw SegmentationFaultException(this->PC + 2, opcode, this->PC);

                this->PC += 2;
            }
            break;
        }
        // Annn - LD I, addr. Set I = nnn.
        case 0xA000: { this->I = nnn; break; }
        // Bnnn - JP V0, addr. Jump to location nnn + V0.
        case 0xB000:
        {
            if ( this->VX[0] + nnn > CHIP8CONF::MEMORY_SIZE )
                throw SegmentationFaultException(this->VX[0] + nnn, opcode, this->PC);

            this->PC = nnn + this->VX[0];
            break;
        }
        // Cxkk - RND Vx, byte. Set Vx = random byte AND kk.
        case 0xC000: { this->VX[x] = std::rand() % (0xFF + 1) & kk; break; }
        // Dxyn - DRW Vx, Vy, nibble. Display n-byte sprite starting at memory location I at (Vx, Vy). VF = collision.
        case 0xD000:
        {
            // Wrap if going beyond screen boundaries
            const uint8_t xPos = this->VX[x] % CHIP8CONF::DISPLAY_WIDTH;
            const uint8_t yPos = this->VX[y] % CHIP8CONF::DISPLAY_HEIGHT;

            this->VX[CHIP8CONF::FLAG_REGISTER_INDEX] = 0x00;
            // height = n
            for ( uint8_t row = 0; row < n; ++row)
            {
                const uint8_t spriteByte = memory[this->I + row];

                for ( uint8_t col = 0; col < 8; ++col )
                {
                    const uint8_t spritePixel = spriteByte & (0x80u >> col);
                    uint8_t* screenPixel = &GB[(xPos + col)][(yPos + row)];

                    // Sprite pixel is on
                    if ( spritePixel )
                    {
                        // Screen pixel also on - collision
                        if (*screenPixel == 0xFF)
                            this->VX[CHIP8CONF::FLAG_REGISTER_INDEX] = 1;

                        // Effectively XOR with the sprite pixel
                        *screenPixel ^= 0xFF;
                    }
                }
            }
            break;
        }
        case 0xE000:
        {
            switch (kk)
            {
                // Ex9E - SKP Vx. Skip next instruction if key with the value of Vx is pressed.
                //      Checks the keyboard, and if the key corresponding to the value of Vx
                //      is currently in the down position, PC is increased by 2.
                case 0x9E:
                {
                    if ( this->KB[this->VX[x]] == 0x01  )
                        this->PC += 2;
                    break;
                }
                // ExA1 - SKNP Vx
                //      Skip next instruction if key with the value of Vx is not pressed.
                //      Checks the keyboard, and if the key corresponding to the value of Vx
                //      is currently in the up position, PC is increased by 2.
                case 0xA1:
                {
                    if ( this->KB[this->VX[x]] == 0x00  )
                        this->PC += 2;
                    break;
                }
                default:
                    throw IllegalInstructionException(opcode, this->PC);
            }
            break;
        }
        case 0xF000:
        {
            switch (kk)
            {
                // Fx07 - LD Vx, DT. Set Vx = delay timer value.
                case 0x07: { this->VX[x] = this->DT;  break; }
                // Fx0A - LD Vx, K. Wait for a key press, store the value of the key in Vx.
                case 0x0A:
                {
                    for ( uint8_t i = 0; i < CHIP8CONF::REGISTER_SIZE; i++ )
                    {
                        if ( KB[i] != 0x00 )
                            this->VX[x] = i;
                    }
                    // Wait is the return from the previous instruction.
                    this->PC -= 2;
                    break;
                }
                // Fx15 - LD DT, Vx. Set delay timer = Vx.
                case 0x15: { this->DT = this->VX[x]; break; }
                // Fx18 - LD ST, Vx. Set sound timer = Vx.
                case 0x18: { this->ST = this->VX[x]; break; }
                // Fx1E - ADD I, Vx. Set I = I + Vx.
                case 0x1E: { this->I += this->VX[x]; break; }
                // Fx29 - LD F, Vx. Set I = location of sprite for digit Vx.
                case 0x29:
                {
                    this->I = CHIP8CONF::FONT_START_ADDRESS + (this->VX[x] & 0x0F) * CHIP8CONF::FONT_CHAR_SIZE;
                    break;
                }
                // Fx33 - LD B, Vx. Store BCD representation of Vx in memory locations I, I+1, and I+2.
                case 0x33:
                {
                    const uint8_t value = this->VX[x];
                    if ( this->I + 2 > CHIP8CONF::MEMORY_SIZE )
                        throw SegmentationFaultException(this->I + 2, opcode, this->PC);

                    this->memory[this->I] = value / 100;
                    this->memory[this->I+1] = value / 10 % 10;
                    this->memory[this->I+2] = value  % 10;
                    break;
                }
                // Fx55 - LD [I], Vx. Store registers V0 through Vx in memory starting at location I.
                case 0x55:
                {
                    // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
                    for ( uint8_t i = 0; i <= x; i++ )
                        this->memory[this->I + i] = this->VX[i];

                    break;
                }
                // Fx65 - LD Vx, [I]. Read registers V0 through Vx from memory starting at location I.
                case 0x65:
                {
                    // The interpreter reads values from memory starting at location I into registers V0 through Vx.
                    for ( uint8_t i = 0; i <= x; i++ )
                        this->VX[i] = this->memory[this->I + i];

                    break;
                }
                default:
                    throw IllegalInstructionException(opcode, this->PC);
            }
            break;
        }
        default:
            throw IllegalInstructionException(opcode, this->PC);

    }

}

void Chip8::fetchAndExec() {
    const uint8_t msbOpCode = this->memory[this->PC];
    const uint8_t lsbOpCode = this->memory[this->PC + 1];
    const uint16_t opcode = msbOpCode << 8 | lsbOpCode;
    this->PC += 2;
    this->execute(opcode);
}

void Chip8::load( const std::string &filename ) {
    std::ifstream rom(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!rom)
        throw ROMLoadingException("Cannot Open ROM File: " + filename);

    const std::streamsize size = rom.tellg();
    constexpr std::size_t maxSize = sizeof(this->memory) - CHIP8CONF::START_PROGRAM_ADDRESS;

    if (size <= 0)
        throw ROMLoadingException("ROM is empty: " + filename);
    if ( static_cast<std::size_t>(size) > maxSize )
        throw ROMLoadingException("Too Large ROM");

    rom.seekg(0, std::ios::beg);
    if ( !rom.read(reinterpret_cast<char*>(&this->memory[CHIP8CONF::START_PROGRAM_ADDRESS]), size) )
        throw ROMLoadingException("Error on ROM Reading");
}

void Chip8::reset() {
    for ( uint8_t i = 0; i < CHIP8CONF::REGISTER_SIZE; i++ )
        this->VX[i] = 0x00;

    for ( uint8_t i = 0; i < CHIP8CONF::STACK_SIZE; i++ )
        this->stack[i] = 0x00;

    this->I = 0;
    this->ST = 0;
    this->DT = 0;
    this->SP = 0;
    this->PC = CHIP8CONF::START_PROGRAM_ADDRESS;

    for ( uint8_t i = 0; i < CHIP8CONF::DISPLAY_WIDTH; i++ )
        for ( uint8_t j = 0; j < CHIP8CONF::DISPLAY_HEIGHT; j++ )
            this->GB[i][j] = 0;
}

void Chip8::updateTimers() {
    if (this->DT > 0) --this->DT;
    if (this->ST > 0) --this->ST;
}