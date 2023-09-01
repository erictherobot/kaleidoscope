#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>

constexpr float PI = 3.14159265359;
constexpr int CHAR_BUFFER_SIZE = 80;
constexpr float INITIAL_ROTATION = 0.0f;
constexpr float TIME_INCREMENT = 0.001f;
constexpr float LINE_THICKNESS = 0.05f;

sf::Color rainbowColor(float i, float total) {
    float frequency = 2.0 * PI / total;
    int r = std::sin(frequency * i + 0) * 127 + 128;
    int g = std::sin(frequency * i + 2) * 127 + 128;
    int b = std::sin(frequency * i + 4) * 127 + 128;
    return sf::Color(r, g, b);
}

std::string getCurrentTime() {
    time_t rawTime;
    struct tm* timeInfo;
    char buffer[CHAR_BUFFER_SIZE];

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(buffer, sizeof(buffer), "%I:%M:%S %p", timeInfo);
    return std::string(buffer);
}

int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "Kaleidoscope", sf::Style::Fullscreen);

    float rotation = INITIAL_ROTATION;
    float time = INITIAL_ROTATION;

    sf::Font font;
    if (!font.loadFromFile("/Library/Fonts/OpenSans-Bold.ttf")) {
        return -1;
    }
    
    sf::Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(24);

    sf::View view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    bool isZoomed = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        window.clear();

        time += TIME_INCREMENT;
        rotation += TIME_INCREMENT;

        int numLines = 3 + 2 * std::sin(time / 4);
        float angleIncrement = 2.0 * PI / numLines;
        float radius = 50 * std::sin(time / 2);

        sf::Vector2f center = sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2);

        for (int i = 0; i < numLines; ++i) {
            float angle = i * angleIncrement + rotation;
            sf::Vector2f start = center;
            sf::Vector2f end = sf::Vector2f(
                center.x + radius * std::cos(angle),
                center.y + radius * std::sin(angle)
            );

            sf::Vector2f direction = end - start;

            sf::RectangleShape line(sf::Vector2f(std::sqrt(direction.x * direction.x + direction.y * direction.y), LINE_THICKNESS));
            line.setPosition(start);
            line.setFillColor(rainbowColor(i + time * 10, numLines));
            line.setRotation(std::atan2(direction.y, direction.x) * 180 / PI);

            window.draw(line);
        }

        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        float distanceToCenter = std::sqrt(
            std::pow(mousePosition.x - center.x, 2) + 
            std::pow(mousePosition.y - center.y, 2)
        );

        if (distanceToCenter < radius) {
            if (!isZoomed) {
                view.zoom(0.5f);
                window.setView(view);
                isZoomed = true;
            }
            timeText.setString(getCurrentTime());
            timeText.setPosition(mousePosition.x / 2, mousePosition.y / 2);
            window.draw(timeText);
        } else {
            if (isZoomed) {
                view.zoom(0.08f);
                window.setView(view);
                isZoomed = false;
            }
        }

        window.display();
    }

    return 0;
}