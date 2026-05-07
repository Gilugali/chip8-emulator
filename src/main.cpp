#include "chip8.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: chip8 <rom>\n";
        return 1;
    }

    Chip8 chip8;
    chip8.loadROM(argv[1]);

    std::cout << "ROM loaded: " << argv[1] << "\n";
    return 0;
}
