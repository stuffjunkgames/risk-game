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

int main()
{
	// do client stuff...

	//////////// SETUP /////////////

	// create world
	sf::Font armyFont = loadFont("arial.ttf");
	World world(armyFont);
	sf::Texture mapTexture;
	if (!mapTexture.loadFromFile("new_map.png"))
		return EXIT_FAILURE;
	world.mapSprite = sf::Sprite(mapTexture);
	world.mapSprite.setScale(1.5, 1.5);
	sf::Texture greyTexture;
	if (!greyTexture.loadFromFile("grey.png"))
		return EXIT_FAILURE;
	world.greySprite = sf::Sprite(greyTexture);
	world.greySprite.setScale(1.5, 1.5);
	sf::Texture normalBordersTexture;
	if (!normalBordersTexture.loadFromFile("normal_borders.png"))
		return EXIT_FAILURE;
	world.normalBordersSprite = sf::Sprite(normalBordersTexture);
	world.normalBordersSprite.setScale(1.5, 1.5);


	unsigned int port = 12345;
	std::string address = "msquared169.ddns.net";

	// let user choose to use a different address
	std::cout << "Use default address \"msquared169.ddns.net\"? (Y/N): ";
	std::string input;
	std::cin >> input;
	if (input == "n" || input == "N")
	{
		std::cout << "Enter new address: ";
		std::cin >> address;
	}

	// connect to server
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect(address, port);
	if(status != sf::Socket::Done)
    {
        // error
        std::cout << "Could not connect to the server!" << std::endl;
		return EXIT_FAILURE;
    }
	std::cout << "Connected to Server!" << std::endl;
	socket.setBlocking(false);

    // set socket to non-blocking?

	// set up start screen options
	// get other player configurations and add them to the world
	// get add commands for assigning territories
	sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Uncertainty");
	GameState gameState;
	gameState.myID = StartScreen(window, world, gameState, socket);
	if (gameState.myID < 0)
	{
		return EXIT_FAILURE;
	}


	//////////// GAMEPLAY ///////////

	for (unsigned int i = 0; i < world.PlayerNumber(); i++) {
		gameState.playerLabels.push_back(Label(armyFont));
		gameState.playerLabels.at(i).setFillColor(world.getPlayer(i)->getColor());
		gameState.playerLabels.at(i).setOutlineThickness(3);
		gameState.playerLabels.at(i).setCharacterSize(30);
		gameState.playerLabels.at(i).setString(world.getPlayer(i)->getName() + ":");
		gameState.playerLabels.at(i).setPosition(sf::Vector2f(GAME_WIDTH - 300, 40 * i));
	}

	gameState.dashedLine = DashedLine();

	gameState.winnerLabel = Label(armyFont);

	std::vector<Button> buttons;
	buttons.push_back(Button(armyFont, "+", sf::Vector2f(0, 0)));// plus
	buttons.push_back(Button(armyFont, "-", sf::Vector2f(0, 0)));// minus
	buttons.push_back(Button(armyFont, "Attack", sf::Vector2f(0, 0), 80, 30)); // attack
	buttons.push_back(Button(armyFont, "PLACE", sf::Vector2f(0, 0), 275, 60)); // phase
	buttons.back().setCharacterSize(40);
	buttons.back().setFillColor(world.getPlayerID(gameState.currentPlayerID)->getColor());
	buttons.push_back(Button(armyFont, ">", sf::Vector2f(0, 0), 60, 60));// change phase
	buttons.back().setCharacterSize(40);
	buttons.back().moveToPosition(sf::Vector2f(275, 0));
	buttons.push_back(Button(armyFont, sf::Vector2f(0, 0), 60, 30, 3));// textbox
	buttons.push_back(Button());// start
	buttons.push_back(Button());// color picker
	buttons.push_back(Button(armyFont, "EXIT", sf::Vector2f(GAME_WIDTH - 100, GAME_HEIGHT - 40), 100, 40));// exit

	gameState.territoryCombo.create(GAME_WIDTH, GAME_HEIGHT);

	if (!loadImages(&world))
	{
		return EXIT_FAILURE;
	}
	for (unsigned int i = 0; i < world.TerritoryNumber(); i++)
	{
		world.getTerritory(i)->borderSprite = sf::Sprite(world.getTerritory(i)->borderTexture);
		world.getTerritory(i)->borderSprite.setScale(1.5, 1.5);
		world.getTerritory(i)->territorySprite = sf::Sprite(world.getTerritory(i)->territoryTexture);
		world.getTerritory(i)->territorySprite.setScale(1.5, 1.5);
		world.getTerritory(i)->territoryImage = world.getTerritory(i)->territoryTexture.copyToImage();

		world.getTerritory(i)->UpdateColor();
		gameState.territoryCombo.draw(world.getTerritory(i)->territorySprite);
	}
	gameState.territoryCombo.display();
	gameState.territoryComboSprite.setTexture(gameState.territoryCombo.getTexture());

	HoverText hoverText(armyFont);
	int chatH = 400;
	ChatBox chat(world.getPlayerID(gameState.myID), armyFont, sf::Vector2f(0, GAME_HEIGHT - chatH), 300, chatH);
	World initialWorld = world;

	sf::Text fpsText;
	fpsText.setFont(armyFont);
	fpsText.setCharacterSize(15);
	fpsText.setPosition(sf::Vector2f(400,0));
	fpsText.setFillColor(sf::Color::White);

	std::srand(std::time(0));

	sf::Time dt;
	sf::Time t;
	sf::Clock clock;

	while (window.isOpen())
	{
		if (GetGameEvents(window, world, buttons, gameState, hoverText, chat, armyFont) < 0)
		{
			sf::Packet packet = ClientRequestResign();
			socket.send(packet);
			return EXIT_FAILURE;
		}
		// draw function called at 60fps
		dt = clock.restart();
		t += dt;
		if (t.asMilliseconds() >= 16)
		{
			fpsText.setString("UPS: " + std::to_string(1000000.0/dt.asMicroseconds()) +
				"  FPS: " + std::to_string(1000.0/t.asMilliseconds()));
			t = sf::Time::Zero;
			DrawGameScreen(window, world, buttons, gameState, hoverText, chat, fpsText);
		}
		GameLogic(world, initialWorld, buttons, gameState, socket, chat);

	}

	return 0;
}
