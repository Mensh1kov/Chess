#include "Button.h"
#include "iostream"

Button::Button(float x, float y, float width, float height, const std::string& text, sf::Color color, sf::Texture * texture) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

//установка текста
    this->font = new sf::Font();
    if (!this->font->loadFromFile("./Font/calibri.ttf")) {
        std::cout << "Error button load font!" << std::endl;
    }
    float size_text = 24;
    this->text.setFont(*this->font);
    this->text.setString(text);
    this->text.setCharacterSize(24);
    this->text.setFillColor(sf::Color::White);
    this->text.setOrigin(this->text.getLocalBounds().width / 2, this->text.getLocalBounds().height / 2);
    this->text.setPosition(x + width / 2, y + height / 2 - 7);
//
    this->button = sf::RectangleShape(sf::Vector2f(width, height));
    this->button.setPosition(x, y);
    this->button.setFillColor(color);
    this->button.setTexture(texture);
}

bool Button::IsPresse(float click_x, float click_y) {
    if ((click_x >= x && click_x <= x + width) && (click_y >= y && click_y <= y + height)) {
        click = !click;
        return true;
    } else
        return false;
}

void Button::PrintButton(sf::RenderWindow &window) {
    window.draw(button);
    window.draw(text);
}

