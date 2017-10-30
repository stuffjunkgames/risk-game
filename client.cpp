#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"
#include "network_screens.hpp"

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1800
#define GAME_HEIGHT 1012

// client code

int RunClient()
{
	// do client stuff...

	//////////// SETUP /////////////

	// create world
	sf::Font armyFont = loadFont("arial.ttf");
	World world(armyFont);

	unsigned int port = 12345;
	std::string address = "msquared169.ddns.net";

	// connect to server
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect(address, port);
	if(status != sf::Socket::Done)
    {
        // error
        return -1;
    }

    // set socket to non-blocking?

	// set up start screen options
	// get other player configurations and add them to the world
	// get add commands for assigning territories
	sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Uncertainty");
	GameState state;
	int myID;
	myID = StartScreen(window, world, state, socket);


	//////////// GAMEPLAY ///////////

	std::vector<Button> buttons;
	buttons.push_back(Button(armyFont, "+", sf::Vector2f(0, 0)));// plus
	buttons.push_back(Button(armyFont, "-", sf::Vector2f(0, 0)));// minus
	buttons.push_back(Button(armyFont, "Attack", sf::Vector2f(0, 0), 80, 30)); // attack
	buttons.push_back(Button(armyFont, "PLACE", sf::Vector2f(0, 0), 275, 60)); // phase
	buttons.back().setCharacterSize(40);
	buttons.back().setFillColor(world.getPlayerID(state.currentPlayerID)->getColor());
	buttons.push_back(Button(armyFont, ">", sf::Vector2f(0, 0), 60, 60));// change phase
	buttons.back().setCharacterSize(40);
	buttons.back().moveToPosition(sf::Vector2f(275, 0));
	buttons.push_back(TextEntry(armyFont, sf::Vector2f(0, 0), 60, 30, 3));// textbox
	buttons.push_back(Button());// start
	buttons.push_back(Button());// color picker
	buttons.push_back(Button(armyFont, "EXIT", sf::Vector2f(GAME_WIDTH - 100, GAME_HEIGHT - 40), 100, 40));// exit

	HoverText hoverText(armyFont);
	World initialWorld = world;

	while (window.isOpen())
	{
		GetGameEvents(window, world, buttons, state, hoverText);
		DrawGameScreen(window, world, buttons, hoverText);
		GameLogic(world, initialWorld, buttons, state);
		
	}
	
	return 0;
}
