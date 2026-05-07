#pragma once
#include <array>
#include <cstdint>
#include <string>

constexpr int MEMORY_SIZE    = 4096;
constexpr int NUM_REGISTERS  = 16;
constexpr int STACK_SIZE     = 16;
constexpr int DISPLAY_WIDTH  = 64;
constexpr int DISPLAY_HEIGHT = 32;
constexpr int NUM_KEYS       = 16;
constexpr int FONTSET_START  = 0x050;
constexpr int ROM_START      = 0x200;

class Chip8 {
public:
    Chip8();

    void loadROM(const std::string& path);
    void cycle();

    std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> display{};
    bool drawFlag = false;

    std::array<uint8_t, NUM_KEYS> keys{};

private:
    std::array<uint8_t, MEMORY_SIZE>   memory{};
    std::array<uint8_t, NUM_REGISTERS> V{};
    uint16_t I   = 0;
    uint16_t PC  = ROM_START;
    uint8_t  SP  = 0;
    std::array<uint16_t, STACK_SIZE> stack{};

    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;

    void loadFontset();
    void executeOpcode(uint16_t opcode);
};
