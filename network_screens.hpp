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

class GameState
{
    int currentPlayerID;
    TurnPhase phase;

    ButtonValues buttonVal;
    int keyPressed;

    int activeTerritory;
    int targetTerritory;

    std::vector<Transfer> transfers;
    Transfer *activeTransfer;
};

sf::Font loadFont(std::string path);
bool loadImages(World *world);

std::string StartScreen(sf::RenderWindow &window, sf::Color &playerColor);

// can we contain all the necessary drawing information in world and a button vector?
int DrawGameScreen(sf::RenderWindow &window, World &world, std::vector<Button> buttons, HoverText hoverText);

// Event polling for game screen
int GetGameEvents(sf::RenderWindow &window, std::vector<Button> &buttons, GameState &gameState, HoverText hoverText);

// game logic
int GameLogic(World &world, World &initialWorld, std::vector<Button> &buttons, GameState &gameState);

#endif // NETWORK_SCREENS_HPP_INCLUDED
