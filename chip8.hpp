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

    // uint8_t screen[256];
    uint8_t screen[256] = {};

    // TODO: decrement
    uint8_t delay_timer;
    uint8_t sound_timer;

    void loadROM(char* filename);
    void loadFont();
    void update();

private:
    void blit(uint8_t x, uint8_t y, uint8_t n);
    uint16_t m_fontOffset = 0x10;
};
