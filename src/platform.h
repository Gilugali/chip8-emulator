#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <cstdint>

constexpr int SCALE = 10;

class Platform {
public:
    Platform(const char* title, int width, int height);

    bool processEvents(std::array<uint8_t, 16>& keys);
    void render(const std::array<uint8_t, 64 * 32>& display);

private:
    sf::RenderWindow window;
    sf::Texture      texture;
    sf::Sprite       sprite;
    std::array<sf::Uint8, 64 * 32 * 4> pixels{};
};
