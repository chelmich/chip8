#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.hpp"

void Chip8::loadROM(char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr) {
        fprintf(stderr, "ERROR: failed to open file\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long filelen = ftell(fp);
    rewind(fp);

    if (filelen > 4096 - 0x200) {
        fprintf(stderr, "ERROR: program too large to load\n");
        exit(1);
    }

    fread(mem + 0x200, filelen, 1, fp);

    fclose(fp);
}

void Chip8::loadFont() {
    const uint8_t fontData[] = {
        0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
        0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
        0x90, 0x90, 0xf0, 0x10, 0x10, // 4
        0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
        0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
        0xf0, 0x10, 0x20, 0x40, 0x40, // 7
        0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
        0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
        0xf0, 0x90, 0xf0, 0x90, 0x90, // A
        0xe0, 0x90, 0xe0, 0x90, 0xe0, // B
        0xf0, 0x80, 0x80, 0x80, 0xf0, // C
        0xe0, 0x90, 0x90, 0x90, 0xe0, // D
        0xf0, 0x80, 0xf0, 0x80, 0xf0, // E
        0xf0, 0x80, 0xf0, 0x80, 0x80  // F
    };

    int dataLen = sizeof(fontData)/sizeof(uint8_t);
    for (int i = 0; i < dataLen; i++) {
        mem[m_fontOffset + i] = fontData[i];
    }
}

void print_disassembly(uint16_t instruction) {
    uint8_t opcode = (instruction & 0xf000) >> 12;

    uint8_t x_reg = (instruction & 0x0f00) >> 8; // x register arg.
    uint8_t y_reg = (instruction & 0x00f0) >> 4; // y register arg.

    uint8_t n = instruction & 0x000f; // low nibble
    uint8_t nn = instruction & 0x00ff; // low byte
    uint16_t nnn = instruction & 0x0fff; // low 3 nibbles

    switch (opcode) {
    case 0x0:
        if (instruction == 0x00e0) {
            printf("clear\n");
            return;
        } else {
            break;
        }
    case 0x1:
        printf("jump 0x%03x\n", nnn);
        return;
    case 0x3:
        printf("skip V%X == 0x%02x\n", x_reg, nn);
        return;
    case 0x4:
        printf("skip V%X != 0x%02x\n", x_reg, nn);
        return;
    case 0x5:
        if (n == 0x0) {
            printf("skip V%X == V%X\n", x_reg, y_reg);
            return;
        } else {
            break;
        }
    case 0x6:
        printf("V%X := 0x%02x\n", x_reg, nn);
        return;
    case 0x8:
        switch (n) {
        case 0x0:
            printf("V%X := V%X\n", x_reg, y_reg);
            return;
        case 0x1:
            printf("V%X |= V%X\n", x_reg, y_reg);
            return;
        case 0x2:
            printf("V%X &= V%X\n", x_reg, y_reg);
            return;
        case 0x3:
            printf("V%X ^= V%X\n", x_reg, y_reg);
            return;
        case 0x4:
            printf("V%X += V%X\n", x_reg, y_reg);
            return;
        case 0x5:
            printf("V%X -= V%X\n", x_reg, y_reg);
            return;
        case 0x6:
            printf("V%X >>= V%X\n", x_reg, y_reg);
            return;
        case 0x7:
            printf("V%X =- V%X\n", x_reg, y_reg);
            return;
        case 0xe:
            printf("V%X <<= V%X\n", x_reg, y_reg);
            return;
        default:
            break;
        }
        break;
    case 0x7:
        printf("V%X += 0x%02x\n", x_reg, nn);
        return;
    case 0x9:
        if (n == 0x0) {
            printf("skip V%X != V%X\n", x_reg, y_reg);
            return;
        } else {
            break;
        }
    case 0xa:
        printf("I := 0x%03x\n", nnn);
        return;
    case 0xb:
        // TODO: use quirk flag here
        printf("jump 0x%03x + V0\n", nnn);
        return;
    case 0xd:
        printf("sprite V%X V%X 0x%x\n", x_reg, y_reg, n);
        return;
    default:
        break;
    }

    printf("Unknown disassembly for instruction: %04x\n", instruction);
}

void Chip8::update() {
    if (pc >= 4096) {
        fprintf(stderr, "ERROR: PC out of bounds\n");
        exit(1);
    }

    uint16_t instruction = mem[pc] << 8 | mem[pc+1]; // fetch
    uint8_t opcode = (instruction & 0xf000) >> 12;

    uint8_t x_reg = (instruction & 0x0f00) >> 8; // x register arg.
    uint8_t y_reg = (instruction & 0x00f0) >> 4; // y register arg.

    uint8_t n = instruction & 0x000f; // low nibble
    uint8_t nn = instruction & 0x00ff; // low byte
    uint16_t nnn = instruction & 0x0fff; // low 3 nibbles

    print_disassembly(instruction);

    switch (opcode) {
    case 0x0:
        if (instruction == 0x00e0) {
            memset(screen, 0, sizeof(screen));
            pc += 2;
            return;
        } else if (instruction == 0x00ee) {
            pc = stack.top();
            stack.pop();
            return;
        } else {
            break;
        }
    case 0x1:
        pc = nnn;
        return;
    case 0x2:
        stack.push(pc + 2);
        pc = nnn;
        return;
    case 0x3:
        if (regs[x_reg] == nn) pc += 4;
        else pc += 2;
        return;
    case 0x4:
        if (regs[x_reg] != nn) pc += 4;
        else pc += 2;
        return;
    case 0x5:
        if (n == 0x0) {
            if (regs[x_reg] == regs[y_reg]) pc += 4;
            else pc += 2;
            return;
        } else {
            break;
        }
    case 0x6:
        regs[x_reg] = nn;
        pc += 2;
        return;
    case 0x7:
        regs[x_reg] += nn;
        pc += 2;
        return;
    case 0x8:
        switch (n) {
        case 0x0:
            regs[x_reg] = regs[y_reg];
            pc += 2;
            return;
        case 0x1:
            regs[x_reg] |= regs[y_reg];
            pc += 2;
            return;
        case 0x2:
            regs[x_reg] &= regs[y_reg];
            pc += 2;
            return;
        case 0x3:
            regs[x_reg] ^= regs[y_reg];
            pc += 2;
            return;
        case 0x4:
            regs[x_reg] += regs[y_reg];
            // TODO: set flags
            pc += 2;
            return;
        case 0x5: {
            uint8_t flag;
            if (regs[x_reg] > regs[y_reg]) flag = 0x1;
            else flag = 0x0;

            regs[x_reg] = regs[x_reg] - regs[y_reg];
            regs[0xf] = flag;
            pc += 2;
            return;
        }
        case 0x6: {
            if (!quirk_shift) regs[x_reg] = regs[y_reg];

            uint8_t flag;
            if (regs[x_reg] & 0x01) flag = 0x1;
            else flag = 0x0;

            regs[x_reg] >>= 1;
            regs[0xf] = flag;
            pc += 2;
            return;
        }
        case 0x7: {
            uint8_t flag;
            if (regs[y_reg] > regs[x_reg]) flag = 0x1;
            else flag = 0x0;

            regs[x_reg] = regs[y_reg] - regs[x_reg];
            regs[0xf] = flag;
            pc += 2;
            return;
        }
        case 0xe: {
            if (!quirk_shift) regs[x_reg] = regs[y_reg];

            uint8_t flag;
            if (regs[x_reg] & 0x80) flag = 0x1;
            else flag = 0x0;

            regs[x_reg] <<= 1;
            regs[0xf] = flag;
            pc += 2;
            return;
        }
        default:
            break;
        }
        break;
    case 0x9:
        if (n == 0x0) {
            if (regs[x_reg] != regs[y_reg]) pc += 4;
            else pc += 2;
            return;
        } else {
            break;
        }
    case 0xa:
        ir = nnn;
        pc += 2;
        return;
    case 0xb:
        if (!quirk_jump_with_offset) {
            pc = nnn + regs[0];
        } else {
            pc = nnn + regs[x_reg];
        }
        return;
    case 0xc:
        regs[x_reg] = rand() & nn;
        pc += 2;
        return;
    case 0xd:
        blit(regs[x_reg], regs[y_reg], n);
        pc += 2;
        return;
    case 0xf:
        switch (nn) {
        case 0x07:
            regs[x_reg] = delay_timer;
            pc += 2;
            return;
        case 0x15:
            delay_timer = regs[x_reg];
            pc += 2;
            return;
        case 0x18:
            sound_timer = regs[x_reg];
            pc += 2;
            return;
        case 0x1e:
            ir += regs[x_reg];
            pc += 2;
            return;
        case 0x29:
            ir = m_fontOffset + (regs[x_reg] * 5);
            pc += 2;
            return;
        case 0x33:
            mem[ir] = regs[x_reg] / 100;
            mem[ir + 1] = (regs[x_reg] / 10) % 10;
            mem[ir + 2] = regs[x_reg] % 10;
            pc += 2;
            return;
        case 0x55:
            for (int i = 0; i <= x_reg; i++) {
                mem[ir + i] = regs[i];
            }
            if (!quirk_regs_load_store) ir += x_reg + 1;
            pc += 2;
            return;
        case 0x65:
            for (int i = 0; i <= x_reg; i++) {
                regs[i] = mem[ir + i];
            }
            if (!quirk_regs_load_store) ir += x_reg + 1;
            pc += 2;
            return;
        default:
            break;
        }
        break;
    default:
        break;
    }

    printf("UNKNOWN INSTRUCTION: %04x\n", instruction);
}

void Chip8::blit(uint8_t xpos, uint8_t ypos, uint8_t num) {
    xpos %= 64;
    ypos %= 32;

    regs[0xf] = 0;

    for (int n = 0; n < num; n++) {
        uint8_t sprite_line = mem[ir+n];
        for (int b = 0; b < 8; b++) {
            if (xpos + b >= 64) break; // stop at screen edge

            bool sprite_bit = sprite_line & (1 << (7 - b));
            if (sprite_bit) {
                int index = ypos * 64 + xpos + b;
                uint8_t screen_bit = 1 << (index % 8);
                uint8_t screen_byte = index / 8;

                // Set flag register
                if (screen[screen_byte] & screen_bit) regs[0xf] = 1;

                // XOR to the screen
                screen[screen_byte] ^= screen_bit;
            }
        }
        ypos++;
        if (ypos >= 32) break; // stop at screen edge
    }
}
