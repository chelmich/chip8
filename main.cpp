#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stack>

#include <SDL2/SDL.h>

void print_registers(uint8_t regs[16]) {
    for (int i = 0; i < 16; i++) {
        printf("V%X  ", i);
    }
    printf("\n");

    for (int i = 0; i < 16; i++) {
        printf("%02X  ", regs[i]);
    }
    printf("\n");
}

void print_memory(uint8_t mem[4096]) {
    int addr = 0;
    while (addr < 4096) {
        printf("0x%03x: ", addr);
        for (int i = 0; i < 8; i++) {
            printf("%02x%02x", mem[addr], mem[addr+1]);
            putchar(i == 7 ? '\n' : ' ');
            addr += 2;
        }
    }
}

void print_usage() {
    printf("Usage: ./chip8 [FILE]\n");
}

class Chip8 {
public:
    uint8_t mem[4096] = {};

    uint8_t regs[16] = {};
    uint16_t pc = 0x200; // program counter
    uint16_t ir = 0; // index register
    std::stack<uint16_t> stack = {};

    // uint8_t screen[256];
    uint8_t screen[256] = {};

    void loadROM(char* filename);
    void update();
    void draw(SDL_Renderer* renderer, int scale) const;
private:
    void blit(uint8_t x, uint8_t y, uint8_t n);
};

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

void print_disassembly(uint16_t instruction) {
    uint8_t opcode = (instruction & 0xf000) >> 12;

    uint8_t x_reg = (instruction & 0x0f00) >> 8; // x register arg.
    uint8_t y_reg = (instruction & 0x00f0) >> 4; // y register arg.

    uint8_t n = instruction & 0x000f; // low nibble
    uint8_t nn = instruction & 0x00ff; // low byte
    uint16_t nnn = instruction & 0x0fff; // low 3 nibbles

    switch(opcode) {
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
    case 0xd:
        printf("sprite V%X V%X 0x%x\n", x_reg, y_reg, n);
        return;
    default:
        break;
    }

    printf("Unknown instruction: %04x\n", instruction);
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

    switch(opcode) {
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
        switch(n) {
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
        case 0x5:
            regs[x_reg] -= regs[y_reg];
            // TODO: set flags
            pc += 2;
            return;
        case 0x6:
            regs[x_reg] = (regs[y_reg] >> 1);
            // TODO: set flags
            pc += 2;
            return;
        case 0x7:
            regs[x_reg] = regs[y_reg] - regs[x_reg];
            // TODO: set flags
            pc += 2;
            return;
        case 0xe:
            regs[x_reg] = (regs[y_reg] << 1);
            // TODO: set flags
            pc += 2;
            return;
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
    case 0xd:
        blit(regs[x_reg], regs[y_reg], n);
        pc += 2;
        return;
    default:
        break;
    }

    printf("Unknown instruction: %04x\n", instruction);
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

void Chip8::draw(SDL_Renderer* renderer, int scale) const {
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            int index = y * 64 + x;
            int byte = index / 8;
            int bit = index % 8;

            if (screen[byte] & (1 << bit)) {
                SDL_Rect rect = {x * scale, y * scale, scale, scale};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)  {
        fprintf(stderr, "ERROR: SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    Chip8 chip;
    chip.loadROM(argv[1]);

    int scale = 8;

    SDL_Window* window = SDL_CreateWindow("chip-8 emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        64 * scale, 32 * scale,
        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED);

    SDL_Color bg_color = {26, 42, 61, 255};
    SDL_Color fg_color = {202, 217, 235, 255};

    bool shouldClose = false;
    while (!shouldClose) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch(event.type) {
        case SDL_QUIT:
            shouldClose = true;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
            case SDLK_ESCAPE:
                shouldClose = true;
                break;
            case SDLK_s:
                chip.update();
                break;
            }
            break;
        }

        SDL_SetRenderDrawColor(renderer,
            bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,
            fg_color.r, fg_color.g, fg_color.b, fg_color.a);
        chip.draw(renderer, scale);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
