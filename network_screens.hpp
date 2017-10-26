#ifndef NETWORK_SCREENS_HPP_INCLUDED
#define NETWORK_SCREENS_HPP_INCLUDED

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

enum ButtonValues {
    PlusButton,
    MinusButton,
    AttackButton,
    ChangePhaseButton,
    TextBox,
    StartButton,
    ColorPicker,
    ExitButton
};

sf::Font loadFont(std::string path);
bool loadImages(World *world);

std::string StartScreen(sf::RenderWindow &window, sf::Color &playerColor);

// can we contain all the necessary drawing information in world and a button vector?
int DrawGameScreen(sf::RenderWindow &window, World &world, std::vector<Button> buttons);

// Event polling for game screen
int GetGameEvents(sf::RenderWindow &window, std::vector<Button> &buttons, ButtonValues &buttonVal, int &keyPressed);

// game logic
// uhh... make a function prototype here...

#endif // NETWORK_SCREENS_HPP_INCLUDED
