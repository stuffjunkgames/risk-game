#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

// only included for loadFont
#include "screens.hpp"

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

	// set up start screen options

	// get other player configurations and add them to the world

	// get add commands for assigning territories

	//////////// GAMEPLAY ///////////

	return 0;
}
