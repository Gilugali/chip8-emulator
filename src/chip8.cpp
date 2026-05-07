#include "chip8.h"
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

static constexpr std::array<uint8_t, 80> FONTSET = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};

Chip8::Chip8() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    loadFontset();
}

void Chip8::loadFontset() {
    for (size_t i = 0; i < FONTSET.size(); ++i)
        memory[FONTSET_START + i] = FONTSET[i];
}

void Chip8::reset() {
    memory.fill(0);
    V.fill(0);
    display.fill(0);
    stack.fill(0);
    keys.fill(0);
    I          = 0;
    PC         = ROM_START;
    SP         = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag   = false;
    loadFontset();
    loadROM(romPath);
}

void Chip8::loadROM(const std::string& path) {
    romPath = path;
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Failed to open ROM: " + path);

    std::streamsize size = file.tellg();
    if (size > static_cast<std::streamsize>(MEMORY_SIZE - ROM_START))
        throw std::runtime_error("ROM too large to fit in memory");

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&memory[ROM_START]), size);
}

void Chip8::cycle() {
    uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
    PC += 2;

    executeOpcode(opcode);

    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}

void Chip8::executeOpcode(uint16_t opcode) {
    uint8_t  x   = (opcode & 0x0F00) >> 8;
    uint8_t  y   = (opcode & 0x00F0) >> 4;
    uint8_t  n   = (opcode & 0x000F);
    uint8_t  kk  = (opcode & 0x00FF);
    uint16_t nnn = (opcode & 0x0FFF);

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {
                display.fill(0);
                drawFlag = true;
            } else if (opcode == 0x00EE) {
                PC = stack[--SP];
            }
            break;

        case 0x1000:
            PC = nnn;
            break;

        case 0x2000:
            stack[SP++] = PC;
            PC = nnn;
            break;

        case 0x3000:
            if (V[x] == kk) PC += 2;
            break;

        case 0x4000:
            if (V[x] != kk) PC += 2;
            break;

        case 0x5000:
            if (V[x] == V[y]) PC += 2;
            break;

        case 0x6000:
            V[x] = kk;
            break;

        case 0x7000:
            V[x] += kk;
            break;

        case 0x8000:
            switch (n) {
                case 0x0: V[x]  = V[y]; break;
                case 0x1: V[x] |= V[y]; break;
                case 0x2: V[x] &= V[y]; break;
                case 0x3: V[x] ^= V[y]; break;
                case 0x4: {
                    uint16_t sum = V[x] + V[y];
                    V[0xF] = sum > 0xFF ? 1 : 0;
                    V[x]   = sum & 0xFF;
                    break;
                }
                case 0x5:
                    V[0xF] = V[x] > V[y] ? 1 : 0;
                    V[x]  -= V[y];
                    break;
                case 0x6:
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    break;
                case 0x7:
                    V[0xF] = V[y] > V[x] ? 1 : 0;
                    V[x]   = V[y] - V[x];
                    break;
                case 0xE:
                    V[0xF] = (V[x] >> 7) & 0x1;
                    V[x] <<= 1;
                    break;
            }
            break;

        case 0x9000:
            if (V[x] != V[y]) PC += 2;
            break;

        case 0xA000:
            I = nnn;
            break;

        case 0xB000:
            PC = nnn + V[0];
            break;

        case 0xC000:
            V[x] = (std::rand() % 256) & kk;
            break;

        case 0xD000: {
            V[0xF] = 0;
            for (int row = 0; row < n; ++row) {
                uint8_t sprite = memory[I + row];
                for (int col = 0; col < 8; ++col) {
                    if (sprite & (0x80 >> col)) {
                        int px = (V[x] + col) % DISPLAY_WIDTH;
                        int py = (V[y] + row) % DISPLAY_HEIGHT;
                        int idx = py * DISPLAY_WIDTH + px;
                        if (display[idx]) V[0xF] = 1;
                        display[idx] ^= 1;
                    }
                }
            }
            drawFlag = true;
            break;
        }

        case 0xE000:
            if (kk == 0x9E && keys[V[x]]) PC += 2;
            else if (kk == 0xA1 && !keys[V[x]]) PC += 2;
            break;

        case 0xF000:
            switch (kk) {
                case 0x07: V[x] = delayTimer; break;
                case 0x0A: {
                    bool pressed = false;
                    for (int i = 0; i < NUM_KEYS; ++i) {
                        if (keys[i]) { V[x] = i; pressed = true; break; }
                    }
                    if (!pressed) PC -= 2;
                    break;
                }
                case 0x15: delayTimer = V[x]; break;
                case 0x18: soundTimer = V[x]; break;
                case 0x1E: I += V[x]; break;
                case 0x29: I = FONTSET_START + (V[x] * 5); break;
                case 0x33:
                    memory[I]     = V[x] / 100;
                    memory[I + 1] = (V[x] / 10) % 10;
                    memory[I + 2] = V[x] % 10;
                    break;
                case 0x55:
                    for (int i = 0; i <= x; ++i) memory[I + i] = V[i];
                    break;
                case 0x65:
                    for (int i = 0; i <= x; ++i) V[i] = memory[I + i];
                    break;
            }
            break;
    }
}
