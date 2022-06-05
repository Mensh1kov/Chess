#include <SFML/network.hpp>
#include <iostream>
#include "Display.h"

//продумать возможность рестарта
int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8; // сглажевание x8
    sf::RenderWindow * window = new sf::RenderWindow(sf::VideoMode(550, 650), "Chess", sf::Style::Default, settings);
    StartMenu * menu = new StartMenu(window);
    Display * display = menu;
    sf::Font * font = new sf::Font;

    window->setFramerateLimit(60);

    while (window->isOpen()) {
        display = display->CheckEvent();
        window->clear(sf::Color::Black);
        display->PrintDisplay();
        window->display();
    }
}
