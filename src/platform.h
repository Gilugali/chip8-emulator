#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <cstdint>
#include <vector>

constexpr int SCALE = 10;

enum class Action { None, Quit, Pause, Reset };

class Platform {
public:
    Platform(const char* title, int width, int height);

    Action processEvents(std::array<uint8_t, 16>& keys);
    void render(const std::array<uint8_t, 64 * 32>& display);
    void updateSound(uint8_t soundTimer);

private:
    sf::RenderWindow window;
    sf::Texture      texture;
    sf::Sprite       sprite;
    std::array<sf::Uint8, 64 * 32 * 4> pixels{};

    sf::SoundBuffer beepBuffer;
    sf::Sound       beep;

    void buildBeep();
};
