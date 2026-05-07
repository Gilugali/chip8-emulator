#include "platform.h"
#include <cmath>

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
    buildBeep();
}

void Platform::buildBeep() {
    constexpr unsigned SAMPLE_RATE = 44100;
    constexpr float    FREQUENCY   = 440.f;
    constexpr float    DURATION    = 0.1f;
    constexpr unsigned SAMPLES     = static_cast<unsigned>(SAMPLE_RATE * DURATION);

    std::vector<sf::Int16> raw(SAMPLES);
    for (unsigned i = 0; i < SAMPLES; ++i)
        raw[i] = static_cast<sf::Int16>(32767 * std::sin(2.f * M_PI * FREQUENCY * i / SAMPLE_RATE));

    beepBuffer.loadFromSamples(raw.data(), SAMPLES, 1, SAMPLE_RATE);
    beep.setBuffer(beepBuffer);
    beep.setLoop(true);
}

Action Platform::processEvents(std::array<uint8_t, 16>& keys) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return Action::Quit;
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) return Action::Quit;
            if (event.key.code == sf::Keyboard::Space)  return Action::Pause;
            if (event.key.code == sf::Keyboard::R)      return Action::Reset;
            for (int i = 0; i < 16; ++i)
                if (event.key.code == KEYMAP[i]) keys[i] = 1;
        }
        if (event.type == sf::Event::KeyReleased) {
            for (int i = 0; i < 16; ++i)
                if (event.key.code == KEYMAP[i]) keys[i] = 0;
        }
    }
    return Action::None;
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

void Platform::updateSound(uint8_t soundTimer) {
    if (soundTimer > 0 && beep.getStatus() != sf::Sound::Playing)
        beep.play();
    else if (soundTimer == 0 && beep.getStatus() == sf::Sound::Playing)
        beep.stop();
}
