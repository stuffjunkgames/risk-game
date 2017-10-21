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

	unsigned int port = 12345;
	std::string address = "msquared169.ddns.net";

	// CLI configurations? (later)

	// load font
	sf::Font armyFont = loadFont("arial.ttf");

	// create listener
	sf::TcpListener listener;
	sf::Packet packet;
	if (listener.listen(port) != sf::Socket::Done)
	{
		std::cout << "Error listening to port!\n";
		return 1;
	}

	listener.setBlocking(false);

	std::unique_ptr<sf::TcpSocket> server = std::unique_ptr<sf::TcpSocket>();

	// TODO: send start signal to server, probably through input from user
	bool started = false;

	// receive configurations from server? (later)


	// get territory assignments from server


	// starting player is picked, sent to clients


	//////////// GAMEPLAY ///////////
	// Wait for commands from current player client
	// check legality
	// send movements to all clients
	// send turn changes to all clients, including next player

	return 0;
}
