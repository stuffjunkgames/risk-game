#ifndef NETWORK_SCREENS_HPP_INCLUDED
#define NETWORK_SCREENS_HPP_INCLUDED

#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>

enum ButtonValues {
    PlusButton,
    MinusButton,
    AttackButton,
	PhaseButton,
    ChangePhaseButton,
    TextBox,
    StartButton,
    ColorPicker,
    ExitButton,
    NoButton
};

class GameState
{
public:
	// constructor
	GameState();

	// functions

	// members
    int currentPlayerID;
    TurnPhase phase;

    ButtonValues buttonVal;
    int keyPressed;
	sf::Vector2f mouseHoverPosition;

    int activeTerritory;
    int targetTerritory;

    std::vector<Transfer> transfers;
    Transfer *activeTransfer;

	std::vector<Label> playerLabels;

	DashedLine dashedLine;
};

sf::Font loadFont(std::string path);
bool loadImages(World *world);
void ResetForNextPhase(TurnPhase nextPhase, World &world, std::vector<Button> &buttons, GameState &state);

// return player number
int StartScreen(sf::RenderWindow &window, World &world, GameState &state, sf::TcpSocket &socket);

// can we contain all the necessary drawing information in world and a button vector?
int DrawGameScreen(sf::RenderWindow &window, World &world, std::vector<Button> &buttons, GameState &gameState, HoverText &hoverText);

// Event polling for game screen
int GetGameEvents(sf::RenderWindow &window, World & world, std::vector<Button> &buttons, GameState &gameState, HoverText &hoverText);

// game logic
int GameLogic(World &world, World &initialWorld, std::vector<Button> &buttons, GameState &gameState);

#endif // NETWORK_SCREENS_HPP_INCLUDED
