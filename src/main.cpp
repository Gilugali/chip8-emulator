#include "chip8.h"
#include "platform.h"
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: chip8 <rom>\n";
        return 1;
    }

    Chip8    chip8;
    Platform platform("CHIP-8", DISPLAY_WIDTH, DISPLAY_HEIGHT);

    chip8.loadROM(argv[1]);

    auto lastCycle = std::chrono::high_resolution_clock::now();
    constexpr int CYCLES_PER_SECOND = 700;

    while (true) {
        if (!platform.processEvents(chip8.keys))
            break;

        auto now     = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastCycle).count();

        int cyclesToRun = static_cast<int>(elapsed * CYCLES_PER_SECOND);
        for (int i = 0; i < cyclesToRun; ++i)
            chip8.cycle();

        if (cyclesToRun > 0)
            lastCycle = now;

        if (chip8.drawFlag) {
            platform.render(chip8.display);
            chip8.drawFlag = false;
        }
    }

    return 0;
}
