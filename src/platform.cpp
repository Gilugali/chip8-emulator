#include "platform.h"

static constexpr std::array<int, 16> KEYMAP = {
    sf::Keyboard::X,
    sf::Keyboard::Num1,
    sf::Keyboard::Num2,
    sf::Keyboard::Num3,
    sf::Keyboard::Q,
    sf::Keyboard::W,
    sf::Keyboard::E,
    sf::Keyboard::A,
    sf::Keyboard::S,
    sf::Keyboard::D,
    sf::Keyboard::Z,
    sf::Keyboard::C,
    sf::Keyboard::Num4,
    sf::Keyboard::R,
    sf::Keyboard::F,
    sf::Keyboard::V,
};

Platform::Platform(const char* title, int width, int height)
    : window(sf::VideoMode(width * SCALE, height * SCALE), title)
{
    window.setFramerateLimit(60);
    texture.create(width, height);
    sprite.setTexture(texture);
    sprite.setScale(SCALE, SCALE);
}

bool Platform::processEvents(std::array<uint8_t, 16>& keys) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return false;
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            return false;
        if (event.type == sf::Event::KeyPressed) {
            for (int i = 0; i < 16; ++i)
                if (event.key.code == KEYMAP[i]) keys[i] = 1;
        }
        if (event.type == sf::Event::KeyReleased) {
            for (int i = 0; i < 16; ++i)
                if (event.key.code == KEYMAP[i]) keys[i] = 0;
        }
    }
    return true;
}

void Platform::render(const std::array<uint8_t, 64 * 32>& display) {
    for (size_t i = 0; i < display.size(); ++i) {
        sf::Uint8 val = display[i] ? 0xFF : 0x00;
        pixels[i * 4 + 0] = val;
        pixels[i * 4 + 1] = val;
        pixels[i * 4 + 2] = val;
        pixels[i * 4 + 3] = 0xFF;
    }
    texture.update(pixels.data());
    window.clear();
    window.draw(sprite);
    window.display();
}
