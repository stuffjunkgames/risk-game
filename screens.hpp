#ifndef SCREENS_HPP_INCLUDED
#define SCREENS_HPP_INCLUDED

#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>

sf::Font loadFont(std::string path);
bool loadImages(World *world);

std::string startScreen(sf::RenderWindow* window, sf::Color* playerColor);
int gameScreen(sf::RenderWindow* window, std::vector<std::string> playerNames, std::vector<sf::Color> playerColors);

#endif // SCREENS_HPP_INCLUDED