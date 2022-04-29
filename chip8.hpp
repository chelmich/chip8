#pragma once

#include <stdint.h>
#include <stack>

class Chip8 {
public:
    uint8_t mem[4096] = {};

    uint8_t regs[16] = {};
    uint16_t pc = 0x200; // program counter
    uint16_t ir = 0; // index register
    std::stack<uint16_t> stack = {};

    uint8_t screen[256] = {};

    // TODO: decrement
    uint8_t delay_timer;
    uint8_t sound_timer;

    // The original interpreter uses VY as the source for the bitshift
    // instructions. Later interpreters ignore it and shift VX in-place.
    bool quirk_shift = false;

    // Some interpreters don't increment the index register for the FX55 and
    // FX65 instructions.
    bool quirk_regs_load_store = false;

    // On some implementations BNNN is interpreted as BXNN, i.e., instead of
    // jumping to NNN + V0 it jumps to NNN + VX.
    bool quirk_jump_with_offset = false;

    void loadROM(char* filename);
    void loadFont();
    void update();

private:
    void blit(uint8_t x, uint8_t y, uint8_t n);
    uint16_t m_fontOffset = 0x10;
};
