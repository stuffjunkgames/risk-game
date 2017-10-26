#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"
#include "network_screens.hpp"

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

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

	//////////// GAMEPLAY ///////////

	return 0;
}
