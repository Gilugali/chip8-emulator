#include "chip8.h"
#include <fstream>
#include <stdexcept>

static constexpr std::array<uint8_t, 80> FONTSET = {
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
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

Chip8::Chip8() {
    loadFontset();
}

void Chip8::loadFontset() {
    for (size_t i = 0; i < FONTSET.size(); ++i)
        memory[FONTSET_START + i] = FONTSET[i];
}

void Chip8::loadROM(const std::string& path) {
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
    // Fetch
    uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
    PC += 2;

    // Decode + Execute
    executeOpcode(opcode);

    // Tick timers
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}

void Chip8::executeOpcode(uint16_t opcode) {
    // Stubbed — opcodes will be implemented next
    (void)opcode;
}
