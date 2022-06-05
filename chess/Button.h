#pragma once
#include "SFML/Graphics.hpp"

class Button {
    bool click = true;
    sf::Font * font;
    sf::Text text;
    float x;
    float y;
    float width;
    float height;
//    sf::Texture * texture;
    sf::RectangleShape button; // времено, должно быть private
public:
    Button(float x, float y, float width, float height, const std::string& text, sf::Color color, sf::Texture * texture);
    bool IsPresse(float x, float y);
    void PrintButton(sf::RenderWindow & window);
};