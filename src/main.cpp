#include "chip8.h"
#include "platform.h"
#include <iostream>
#include <chrono>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: chip8 <rom> [cycles-per-second]\n";
        return 1;
    }

    int cyclesPerSecond = 700;
    if (argc >= 3) {
        try {
            cyclesPerSecond = std::stoi(argv[2]);
            if (cyclesPerSecond <= 0) throw std::invalid_argument("");
        } catch (...) {
            std::cerr << "Invalid cycles-per-second value: " << argv[2] << "\n";
            return 1;
        }
    }

    std::string title = "CHIP-8 — " + std::filesystem::path(argv[1]).filename().string();

    Chip8    chip8;
    Platform platform(title.c_str(), DISPLAY_WIDTH, DISPLAY_HEIGHT);

    chip8.loadROM(argv[1]);

    auto lastCycle = std::chrono::high_resolution_clock::now();
    bool paused    = false;

    while (true) {
        Action action = platform.processEvents(chip8.keys);

        if (action == Action::Quit)  break;
        if (action == Action::Pause) paused = !paused;
        if (action == Action::Reset) {
            chip8.reset();
            lastCycle = std::chrono::high_resolution_clock::now();
            paused    = false;
        }

        if (!paused) {
            auto now     = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double>(now - lastCycle).count();

            int cyclesToRun = static_cast<int>(elapsed * cyclesPerSecond);
            for (int i = 0; i < cyclesToRun; ++i)
                chip8.cycle();

            if (cyclesToRun > 0)
                lastCycle = now;

            platform.updateSound(chip8.getSoundTimer());

            if (chip8.drawFlag) {
                platform.render(chip8.display);
                chip8.drawFlag = false;
            }
        }
    }

    return 0;
}
