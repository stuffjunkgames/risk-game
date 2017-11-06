#include "network_screens.hpp"

#include <sstream>

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1800
#define GAME_HEIGHT 1012
#define NO_KEY_PRESSED -1
#define KEY_PRESSED_ONCE -2

sf::Font loadFont(std::string path)
{
	sf::Font font;
	if (!font.loadFromFile(path))
	{
		// error
		exit(EXIT_FAILURE);
	}

	return font;
}

bool loadImages(World *world)
{
	std::string filename{ "" };
	std::ifstream file("borderImagesList.txt");

	for (int i = 0; file >> filename; i++) {
		world->getTerritory(i)->borderTexture = sf::Texture();
		if (!world->getTerritory(i)->borderTexture.loadFromFile(filename))
			return 0;
	}

	file = std::ifstream("territoryImagesList.txt");

	for (int i = 0; file >> filename; i++) {
		world->getTerritory(i)->territoryTexture = sf::Texture();
		if (!world->getTerritory(i)->territoryTexture.loadFromFile(filename))
			return 0;
	}

	return 1;
}

// return player number
int StartScreen(sf::RenderWindow &window, World &world, GameState &state, sf::TcpSocket &socket)
{
    sf::RectangleShape background = sf::RectangleShape(sf::Vector2f(GAME_WIDTH, GAME_HEIGHT));

    sf::Texture texture;
	if (!texture.loadFromFile("title.png"))
		return -1;
	sf::Sprite title(texture);
	title.setScale(1.5, 1.5);

	background.setPosition(GAME_LEFT, GAME_TOP);
	background.setFillColor(sf::Color(63, 63, 63, 255));

	sf::Font armyFont = loadFont("arial.ttf");

	Button buttonStart(armyFont, "READY", sf::Vector2f(GAME_WIDTH / 2 - 137, GAME_HEIGHT / 2 - 30 + 200), 275, 60);
	buttonStart.setCharacterSize(40);
	Button buttonExit(armyFont, "EXIT", sf::Vector2f(GAME_WIDTH - 100, GAME_HEIGHT - 40), 100, 40);
	ColorPalette colorPalette(sf::Vector2f(GAME_WIDTH / 2 - 200, GAME_HEIGHT / 4 + 75 + 200), 4);
	Label paletteLabel(armyFont);
	paletteLabel.setString("Choose a Color:");
	sf::FloatRect bounds = colorPalette.getLocalBounds();
	paletteLabel.setPosition(sf::Vector2f(bounds.left + bounds.width / 2 - paletteLabel.getLocalBounds().width / 2, bounds.top - paletteLabel.getLocalBounds().height));
	TextEntry textBox(armyFont, sf::Vector2f(GAME_WIDTH / 2 - 150, GAME_HEIGHT / 4 + 200), 300, 30, 20);
	Label textBoxLabel(armyFont);
	textBoxLabel.setString("Choose a Name:");
	bounds = textBox.getLocalBounds();
	textBoxLabel.setPosition(sf::Vector2f(bounds.left + bounds.width / 2 - textBoxLabel.getLocalBounds().width / 2, bounds.top - textBoxLabel.getLocalBounds().height));

	bool isTyping = false;

	ButtonValues buttonPressed = ButtonValues::NoButton;

	std::string playerName = "";

	bool mouseDown = false;
	int keyPressed = NO_KEY_PRESSED;

	std::srand(std::time(0));

	sf::Vector2f mousePosition;

	sf::Time dt;
	sf::Time t;
	sf::Clock clock;

	int id;
	sf::Color playerColor;

	while (window.isOpen())
	{
	    sf::Packet packet;
		mouseDown = false;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::TextEntered)
			{
				if (keyPressed == NO_KEY_PRESSED)
				{
					keyPressed = event.text.unicode;
					std::cout << "Key pressed: " << keyPressed << std::endl;
				}
			}

			if (event.type == sf::Event::KeyReleased)
			{
				keyPressed = NO_KEY_PRESSED;
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					// left mouse pressed
					mouseDown = true;
					mousePosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

					if (buttonStart.isInside(mousePosition) && buttonStart.isActive)
					{
						buttonPressed = ButtonValues::StartButton;
						mouseDown = false;
					}
					else if (textBox.isInside(mousePosition) && textBox.isActive)
					{
						buttonPressed = ButtonValues::TextBox;
						mouseDown = false;
					}
					else if (colorPalette.isInside(mousePosition))
					{
						std::cout << std::to_string(colorPalette.getSelectedColor().r) << ", " << std::to_string(colorPalette.getSelectedColor().g) << ", " << std::to_string(colorPalette.getSelectedColor().b) << std::endl;
						buttonPressed = ButtonValues::ColorPicker;
						mouseDown = false;
					}
					else if (buttonExit.isInside(mousePosition) && buttonExit.isActive)
					{
						buttonPressed = ButtonValues::ExitButton;
						mouseDown = false;
					}
					else
					{
						buttonPressed = ButtonValues::NoButton;
					}

					if (buttonPressed != ButtonValues::TextBox)
					{
						textBox.setOutlineColor(sf::Color::Black);
						isTyping = false;
					}
				}
			}
		}

		// draw
		dt = clock.restart();
		t += dt;
		if (t.asMilliseconds() >= 16)
		{

			t = sf::Time::Zero;

			// draw
			window.clear();
			window.draw(background);
			window.draw(title);

			textBox.Draw(&window);
			window.draw(textBoxLabel);
			buttonStart.Draw(&window);
			buttonExit.Draw(&window);

			colorPalette.Draw(&window);
			window.draw(paletteLabel);

			window.display();
		}

		if (mouseDown)
		{

		}

		if (isTyping)
		{
			if (keyPressed < 127 && keyPressed >= 32)
			{
				textBox.appendString(std::string() + static_cast<char>(keyPressed));
				keyPressed = NO_KEY_PRESSED;
			}
			else if (keyPressed == 8)//backspace
			{
				textBox.remove();
				keyPressed = NO_KEY_PRESSED;
			}
			/*else
			{
				sf::String unicodeStr;
				unicodeStr = sf::String(sf::Uint32(keyPressed));
				textBox.appendString(std::string() + unicodeStr.toAnsiString());
				keyPressed = NO_KEY_PRESSED;
			}*/
		}

		if (buttonPressed == ButtonValues::TextBox)
		{
			textBox.setOutlineColor(sf::Color::Yellow);
			isTyping = true;
			buttonPressed = ButtonValues::NoButton;
		}
		else if (buttonPressed == ButtonValues::StartButton)
		{
		    // check that color and name are picked

			buttonStart.setOutlineColor(sf::Color::Yellow);

			// send ready signal to server
			packet = ClientRequestReady(textBox.getLabel()->getString(), playerColor);
			socket.send(packet);

			keyPressed = NO_KEY_PRESSED;
			buttonPressed = ButtonValues::NoButton;
		}
		else if (buttonPressed == ButtonValues::ColorPicker)
		{
			playerColor.r = colorPalette.getSelectedColor().r;
			playerColor.g = colorPalette.getSelectedColor().g;
			playerColor.b = colorPalette.getSelectedColor().b;
			playerColor.a = colorPalette.getSelectedColor().a;
			buttonPressed = ButtonValues::NoButton;
		}
		else if (buttonPressed == ButtonValues::ExitButton)
		{
		    // TODO: send disconnect signal to server
			buttonPressed = ButtonValues::NoButton;
			return -1;
		}

		// listen for start signals from server
		if(socket.receive(packet) == sf::Socket::Status::Done)
        {
            // decode packet
            std::string s;
            if(packet >> s)
            {
                if(s == "ready")
                {
                    int playerID;
                    std::string name;
                    sf::Uint32 colorRGB;
                    sf::Color color;

                    if(packet >> playerID >> name >> colorRGB)
                    {
                        color = sf::Color(colorRGB);
                        world.addPlayer(playerID, name, color);
                    }
                }
                else if(s == "id")
                {
                    packet >> id;
                }
                else if(s == "add")
                {
                    int target;
                    int army;
                    int owner;

                    if(packet >> target >> army >> owner)
                    {
						world.getPlayerID(owner)->CaptureTerritory(world.getTerritory(target), army);
                    }
                }
                else if(s == "phase")
                {
                    // start screen is done -- start game
					int phase;
					int player;
					if (packet >> phase >> player)
					{
						state.currentPlayerID = player;
						state.phase = (TurnPhase)phase;

						// exit StartScreen
						return id;
					}
                }
            }
        }
	}

	return -1;
}

int DrawGameScreen(sf::RenderWindow & window, World & world, std::vector<Button>& buttons, GameState &gameState, HoverText &hoverText)
{
	// clear the window
	window.clear();
	// draw the underlying map image
	window.draw(world.mapSprite);
	// draw all of the territory images
	for (unsigned int i = 0; i < world.TerritoryNumber(); i++)
	{
		world.getTerritory(i)->drawTerritory(&window);
	}
	// draw the black borders image
	window.draw(world.normalBordersSprite);
	// draw the active territory's yellow border
	if (gameState.activeTerritory >= 0 && gameState.activeTerritory < (int)world.TerritoryNumber()) {
		window.draw(world.getTerritory(gameState.activeTerritory)->borderSprite);
	}

	if (gameState.phase == place)
	{
		if (gameState.activeTerritory >= 0)
		{
			buttons.at(PlusButton).Draw(&window);
			buttons.at(MinusButton).Draw(&window);
		}
		else
		{
			buttons.at(PlusButton).isActive = false;
			buttons.at(MinusButton).isActive = false;
		}
	}
	else if (gameState.phase == attack)
	{
		if (gameState.targetTerritory >= 0)
		{
			gameState.dashedLine.Draw(&window, world.getTerritory(gameState.activeTerritory)->centerPos, world.getTerritory(gameState.targetTerritory)->centerPos);
			buttons.at(AttackButton).Draw(&window);
			buttons.at(TextBox).Draw(&window);
		}
		else if (gameState.activeTerritory >= 0)
		{
			window.draw(world.greySprite);
			for (unsigned int i = 0; i < world.getTerritory(gameState.activeTerritory)->getConnected()->size(); i++)
			{
				if (world.getTerritory(gameState.activeTerritory)->getConnected()->at(i)->GetOwner() != world.getTerritory(gameState.activeTerritory)->GetOwner())
				{
					world.getTerritory(gameState.activeTerritory)->getConnected()->at(i)->drawTerritory(&window);
				}
			}
			world.getTerritory(gameState.activeTerritory)->drawTerritory(&window);
			window.draw(world.normalBordersSprite);
			// draw the active territory's yellow border
			if (gameState.activeTerritory >= 0 && gameState.activeTerritory < (int)world.TerritoryNumber()) {
				window.draw(world.getTerritory(gameState.activeTerritory)->borderSprite);
			}
			gameState.dashedLine.Draw(&window, world.getTerritory(gameState.activeTerritory)->centerPos, gameState.mouseHoverPosition);
			buttons.at(AttackButton).isActive = false;
			buttons.at(TextBox).isActive = false;
		}
		else
		{
			buttons.at(AttackButton).isActive = false;
			buttons.at(TextBox).isActive = false;
		}
	}
	else if (gameState.phase == reposition)
	{
		if (gameState.targetTerritory >= 0)
		{
			gameState.dashedLine.Draw(&window, world.getTerritory(gameState.activeTerritory)->centerPos, world.getTerritory(gameState.targetTerritory)->centerPos);
			buttons.at(PlusButton).Draw(&window);
			buttons.at(MinusButton).Draw(&window);
		}
		else if (gameState.activeTerritory >= 0)
		{
			window.draw(world.greySprite);
			for (unsigned int i = 0; i < world.getTerritory(gameState.activeTerritory)->getConnected()->size(); i++)
			{
				if (world.getTerritory(gameState.activeTerritory)->getConnected()->at(i)->GetOwner() == world.getTerritory(gameState.activeTerritory)->GetOwner())
				{
					world.getTerritory(gameState.activeTerritory)->getConnected()->at(i)->drawTerritory(&window);
				}
			}
			world.getTerritory(gameState.activeTerritory)->drawTerritory(&window);
			window.draw(world.normalBordersSprite);
			// draw the active territory's yellow border
			if (gameState.activeTerritory >= 0 && gameState.activeTerritory < (int)world.TerritoryNumber()) {
				window.draw(world.getTerritory(gameState.activeTerritory)->borderSprite);
			}
			gameState.dashedLine.Draw(&window, world.getTerritory(gameState.activeTerritory)->centerPos, gameState.mouseHoverPosition);
			buttons.at(PlusButton).isActive = false;
			buttons.at(MinusButton).isActive = false;
		}
		else
		{
			buttons.at(PlusButton).isActive = false;
			buttons.at(MinusButton).isActive = false;
		}
		for (Transfer t : gameState.transfers)
		{
			t.Draw(&window);
		}
	}

	for (unsigned int i = 0; i < gameState.playerLabels.size(); i++)
	{
		gameState.playerLabels.at(i).setString(world.getPlayer(i)->getName() + ": " + std::to_string(3 + world.GetBonus(i)));
		window.draw(gameState.playerLabels.at(i));
	}
	buttons.at(PhaseButton).Draw(&window);
	buttons.at(ChangePhaseButton).Draw(&window);
	buttons.at(ExitButton).Draw(&window);

	// draw the hover text
	sf::Color pxColor;
	for (unsigned int i = 0; i < world.TerritoryNumber(); i++)
	{
		pxColor = world.getTerritory(i)->territoryImage.getPixel(gameState.mouseHoverPosition.x / 1.5, gameState.mouseHoverPosition.y / 1.5); // division by 1.5 because everything is scaled up by 1.5
		if (pxColor.a > 0) {
			hoverText.setText(world.getTerritory(i)->getName() + " " + std::to_string(i) +
				"\n" + world.GetBonusName(i) + " " + std::to_string(world.GetBonusIncome(i)),
				gameState.mouseHoverPosition.x, gameState.mouseHoverPosition.y);
			hoverText.Draw(&window);
			break;
		}
	}

	// display everything that was drawn
	window.display();

	return 0;
}

int GetGameEvents(sf::RenderWindow & window, World & world, std::vector<Button>& buttons, GameState & gameState, HoverText & hoverText, sf::Font &armyFont)
{
	sf::Vector2f mousePosition;
	bool mouseDown = false;
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();
		
		// get key presses, respond to them
		if (event.type == sf::Event::KeyPressed)
		{
			//this is only working for one key pressed at a time
			if (gameState.keyPressed == NO_KEY_PRESSED)
			{
				gameState.keyPressed = event.key.code;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			gameState.keyPressed = NO_KEY_PRESSED;
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				// left mouse pressed
				mousePosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

				buttons.at(TextBox).setIsTyping(false);

				// Set flags for button presses
				if (buttons.at(ButtonValues::PlusButton).isInside(mousePosition) && buttons.at(ButtonValues::PlusButton).isActive)
				{
					gameState.buttonVal = ButtonValues::PlusButton;
				}
				else if (buttons.at(ButtonValues::MinusButton).isInside(mousePosition) && buttons.at(ButtonValues::MinusButton).isActive)
				{
					gameState.buttonVal = ButtonValues::MinusButton;
				}
				else if (buttons.at(ButtonValues::AttackButton).isInside(mousePosition) && buttons.at(ButtonValues::AttackButton).isActive)
				{
					gameState.buttonVal = ButtonValues::AttackButton;
				}
				else if (buttons.at(ButtonValues::ChangePhaseButton).isInside(mousePosition) && buttons.at(ButtonValues::ChangePhaseButton).isActive)
				{
					gameState.buttonVal = ButtonValues::ChangePhaseButton;
				}
				else if (buttons.at(ButtonValues::TextBox).isInside(mousePosition) && buttons.at(ButtonValues::TextBox).isActive)
				{
					gameState.buttonVal = ButtonValues::TextBox;
					buttons.at(TextBox).setIsTyping(true);
				}
				else if (buttons.at(ButtonValues::ExitButton).isInside(mousePosition) && buttons.at(ButtonValues::ExitButton).isActive)
				{
					gameState.buttonVal = ButtonValues::ExitButton;

					return -1;
				}
				else
				{
					mouseDown = true;
					buttons.at(TextBox).setIsTyping(false);
					gameState.buttonVal = ButtonValues::NoButton;
					gameState.activeTransfer = nullptr;
					for (std::vector<Transfer>::iterator it = gameState.transfers.begin(); it != gameState.transfers.end(); it++)
					{
						if (it->getAmount() <= 0)
						{
							gameState.transfers.erase(it);
						}
					}
				}
			}
		}
		if (event.type == sf::Event::MouseMoved)
		{
			gameState.mouseHoverPosition = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
		}
	}

	if (mouseDown && gameState.myID == gameState.currentPlayerID)// mouse was clicked, but not on any buttons
	{
		// Check the territory images for non-zero alpha value at mousePosition
		sf::Color pxColor;
		int clickedTerritory = -1;
		for (unsigned int i = 0; i < world.TerritoryNumber() && clickedTerritory == -1; i++) {
			pxColor = world.getTerritory(i)->territoryImage.getPixel(mousePosition.x / 1.5, mousePosition.y / 1.5); // devision by 1.5 because everything is scaled up by 1.5
			if (pxColor.a > 0) {
				clickedTerritory = i;
			}
			else {
				clickedTerritory = -1;
			}
		}
		if (clickedTerritory >= 0)// clicked in a valid territory
		{
			switch (gameState.phase)
			{
			case TurnPhase::place:
			{
				if (*(world.getTerritory(clickedTerritory)->GetOwner()) == *world.getPlayerID(gameState.currentPlayerID))// player clicked their own territory
				{
					if (clickedTerritory != gameState.activeTerritory)// clicked territory is not already active
					{
						gameState.activeTerritory = clickedTerritory;
					}
					else
					{
						gameState.activeTerritory = -1;
					}
				}
				else
				{
					gameState.activeTerritory = -1;
				}
				break;
			}
			case TurnPhase::attack:
			{
				if (*(world.getTerritory(clickedTerritory)->GetOwner()) == *world.getPlayerID(gameState.myID))// player clicked their own territory
				{
					if (clickedTerritory != gameState.activeTerritory)// clicked territory is not already active
					{
						gameState.activeTerritory = clickedTerritory;
					}
					else
					{
						gameState.activeTerritory = -1;
						gameState.targetTerritory = -1;
					}
				}
				else
				{
					if (gameState.activeTerritory >= 0)// active already set
					{
						if (world.getTerritory(clickedTerritory)->isConnected(world.getTerritory(gameState.activeTerritory)))// active and clicked are connected
						{
							gameState.targetTerritory = clickedTerritory;
						}
						else
						{
							gameState.targetTerritory = -1;
						}
					}
					else
					{
						gameState.activeTerritory = -1;
						gameState.targetTerritory = -1;
					}
				}
				if (gameState.activeTerritory >= 0)
				{
					if (gameState.targetTerritory >= 0)
					{
						gameState.dashedLine.setPoints(world.getTerritory(gameState.activeTerritory)->centerPos, world.getTerritory(gameState.targetTerritory)->centerPos);
						buttons.at(AttackButton).moveToPosition(gameState.dashedLine.getCenter());
						buttons.at(TextBox).moveToPosition(sf::Vector2f(buttons.at(AttackButton).getLocalBounds().left + buttons.at(AttackButton).getLocalBounds().width, buttons.at(AttackButton).getLocalBounds().top));
						buttons.at(TextBox).clearText();
						buttons.at(TextBox).appendString("1");
					}
					else
					{
						gameState.dashedLine.setPoints(world.getTerritory(gameState.activeTerritory)->centerPos, gameState.mouseHoverPosition);
					}					
				}
				break;
			}
			case TurnPhase::reposition:
			{
				if (*(world.getTerritory(clickedTerritory)->GetOwner()) == *world.getPlayerID(gameState.myID))// player clicked their own territory
				{
					if (clickedTerritory != gameState.activeTerritory)// clicked territory is not already active
					{
						if (gameState.activeTerritory >= 0)// active already set
						{
							if (gameState.targetTerritory >= 0)// target already set
							{
								gameState.activeTerritory = clickedTerritory;
								gameState.targetTerritory = -1;
							}
							else
							{
								if (world.getTerritory(clickedTerritory)->isConnected(world.getTerritory(gameState.activeTerritory)))// active and clicked are connected
								{
									gameState.targetTerritory = clickedTerritory;
								}
								else
								{
									gameState.activeTerritory = -1;
									gameState.targetTerritory = -1;
								}
							}
						}
						else
						{
							gameState.activeTerritory = clickedTerritory;
						}
					}
					else
					{
						gameState.activeTerritory = -1;
						gameState.targetTerritory = -1;
					}
				}
				else
				{
					gameState.activeTerritory = -1;
					gameState.targetTerritory = -1;
				}

				if (gameState.activeTerritory >= 0)
				{
					if (gameState.targetTerritory >= 0)
					{
						gameState.dashedLine.setPoints(world.getTerritory(gameState.activeTerritory)->centerPos, world.getTerritory(gameState.targetTerritory)->centerPos);
						buttons.at(PlusButton).moveToPosition(gameState.dashedLine.getCenter() + sf::Vector2f(30, -25));
						buttons.at(MinusButton).moveToPosition(gameState.dashedLine.getCenter() + sf::Vector2f(30, 7));

						for (unsigned int i = 0; i < gameState.transfers.size(); i++)
						{
							if (gameState.transfers.at(i).donor == gameState.activeTerritory && gameState.transfers.at(i).receiver == gameState.targetTerritory) {
								gameState.activeTransfer = &gameState.transfers.at(i);
								break;
							}
						}
						if (gameState.activeTransfer == nullptr)
						{
							gameState.transfers.push_back(Transfer(armyFont, gameState.activeTerritory, gameState.targetTerritory, gameState.dashedLine));
							gameState.transfers.back().setLinePoints(world.getTerritory(gameState.activeTerritory)->centerPos, world.getTerritory(gameState.targetTerritory)->centerPos);
							gameState.activeTransfer = &gameState.transfers.back();
						}
					}
					else
					{
						gameState.dashedLine.setPoints(world.getTerritory(gameState.activeTerritory)->centerPos, gameState.mouseHoverPosition);
					}
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}
		else// clicked ouside all valid territories
		{
			gameState.activeTerritory = -1;
			gameState.targetTerritory = -1;
		}
	}

	if (buttons.at(TextBox).isTyping && gameState.myID == gameState.currentPlayerID)
	{
		if (gameState.keyPressed >= sf::Keyboard::Num0 && gameState.keyPressed <= sf::Keyboard::Num9)
		{
			buttons.at(TextBox).appendString(std::to_string(gameState.keyPressed - sf::Keyboard::Num0));

			gameState.keyPressed = KEY_PRESSED_ONCE;
		}
		else if (gameState.keyPressed >= sf::Keyboard::Numpad0 && gameState.keyPressed <= sf::Keyboard::Numpad9)
		{
			buttons.at(TextBox).appendString(std::to_string(gameState.keyPressed - sf::Keyboard::Numpad0));

			gameState.keyPressed = KEY_PRESSED_ONCE;
		}
		else if (gameState.keyPressed == sf::Keyboard::BackSpace)
		{
			buttons.at(TextBox).remove();

			gameState.keyPressed = KEY_PRESSED_ONCE;
		}
	}

	return 0;
}

int GameLogic(World & world, World & initialWorld, std::vector<Button>& buttons, GameState & gameState, sf::TcpSocket &socket)
{
	Player* currentPlayer = world.getPlayerID(gameState.currentPlayerID);
	sf::Packet packet;

	// send packet to server requesting a move, get return packet, call appropriate movement.cpp command
	switch (gameState.phase)
	{
	case place:
	{
		if (gameState.activeTerritory >= 0 && world.getTerritory(gameState.activeTerritory)->GetOwner()->getID() == gameState.myID)
		{
			buttons.at(PlusButton).moveToPosition(world.getTerritory(gameState.activeTerritory)->centerPos + sf::Vector2f(30, -25));
			buttons.at(MinusButton).moveToPosition(world.getTerritory(gameState.activeTerritory)->centerPos + sf::Vector2f(30, 7));

			if (gameState.keyPressed == sf::Keyboard::Add || gameState.keyPressed == sf::Keyboard::Equal || gameState.buttonVal == ButtonValues::PlusButton) // + key
			{
				packet = ClientRequestAdd(gameState.activeTerritory, 1);
				socket.send(packet);
				
				if (gameState.buttonVal == ButtonValues::PlusButton)
				{
					gameState.keyPressed = NO_KEY_PRESSED;
				}
				else
				{
					gameState.keyPressed = KEY_PRESSED_ONCE;
				}
				gameState.buttonVal = ButtonValues::NoButton;
			}
			else if (gameState.keyPressed == sf::Keyboard::Subtract || gameState.keyPressed == sf::Keyboard::Dash || gameState.buttonVal == ButtonValues::MinusButton) // - key
			{
				packet = ClientRequestAdd(gameState.activeTerritory, -1);
				socket.send(packet);

				if (gameState.buttonVal == ButtonValues::MinusButton)
				{
					gameState.keyPressed = NO_KEY_PRESSED;
				}
				else
				{
					gameState.keyPressed = KEY_PRESSED_ONCE;
				}
				gameState.buttonVal = ButtonValues::NoButton;
			}
		}

		if (gameState.keyPressed == sf::Keyboard::Return || gameState.buttonVal == ButtonValues::ChangePhaseButton)
		{
			packet = ClientRequestPhaseChange();
			socket.send(packet);
			gameState.buttonVal = ButtonValues::NoButton;
		}

		break;
	}
	case attack:
	{
		if (gameState.buttonVal == ButtonValues::AttackButton)
		{
			std::stringstream ss((buttons.at(TextBox).getLabel()->getString()));
			int attackNumber;
			if (ss >> attackNumber)
			{
				packet = ClientRequestMove(gameState.activeTerritory, gameState.targetTerritory, attackNumber);
				socket.send(packet);
			}
			gameState.buttonVal = ButtonValues::NoButton;
		}

		if (gameState.keyPressed == sf::Keyboard::Return || gameState.buttonVal == ButtonValues::ChangePhaseButton)
		{
			packet = ClientRequestPhaseChange();
			socket.send(packet);
			gameState.buttonVal = ButtonValues::NoButton;
		}

		break;
	}
	case reposition:
	{
		if ((gameState.keyPressed == sf::Keyboard::Add || gameState.keyPressed == sf::Keyboard::Equal || gameState.buttonVal == ButtonValues::PlusButton) // + pressed
			&& gameState.targetTerritory >= 0 // target territory exists
			&& initialWorld.getTerritory(gameState.activeTerritory)->GetArmies() > 1)  // source territory has unmoved armies
		{
			initialWorld.getTerritory(gameState.activeTerritory)->AddArmies(-1);    // remove army from initial distribution
			gameState.activeTransfer->increaseAmount(1);
			gameState.buttonVal = ButtonValues::NoButton;
		}
		if ((gameState.keyPressed == sf::Keyboard::Dash || gameState.keyPressed == sf::Keyboard::Subtract || gameState.buttonVal == ButtonValues::PlusButton) // + pressed
			&& gameState.targetTerritory >= 0 // target territory exists
			&& gameState.activeTransfer->getAmount() > 0)  // a transfer has been commanded
		{
			initialWorld.getTerritory(gameState.activeTerritory)->AddArmies(1);    // remove army from initial distribution
			gameState.activeTransfer->increaseAmount(-1);
			gameState.buttonVal = ButtonValues::NoButton;
		}

		if (gameState.keyPressed == sf::Keyboard::Return || gameState.buttonVal == ButtonValues::ChangePhaseButton)
		{
			for (Transfer t : gameState.transfers)
			{
				packet = ClientRequestMove(t.donor, t.receiver, t.getAmount());
				socket.send(packet);
			}

			packet = ClientRequestPhaseChange();
			socket.send(packet);
			gameState.buttonVal = ButtonValues::NoButton;
		}

		break;
	}
	default:
	{
		// something broke...
		break;
	}
	}

	

	// listen for any command signals from server
	if (socket.receive(packet) == sf::Socket::Status::Done)
	{
		// decode packet
		std::string s;
		if (packet >> s)
		{
			if (s == "add")
			{
				int target, army, owner;
				if (packet >> target >> army >> owner)
				{
					world.getTerritory(target)->AddArmies(army);
				}
			}
			else if (s == "move")
			{
				int source, sourceRemaining, target, targetRemaining, targetOwner;
				if (packet >> source >> sourceRemaining >> target >> targetRemaining >> targetOwner)
				{
					world.getPlayerID(targetOwner)->CaptureTerritory(world.getTerritory(target), targetRemaining);
					world.getTerritory(source)->AddArmies(sourceRemaining - world.getTerritory(source)->GetArmies());
				}
			}
			else if (s == "phase")
			{
				int phase, player;
				if (packet >> phase >> player)
				{
					gameState.currentPlayerID = player;
					gameState.phase = (TurnPhase)phase;
					ResetForNextPhase(world, buttons, gameState);
					initialWorld = world;
				}
			}
		}
	}

	return 0;
}



GameState::GameState()
{
	currentPlayerID = -1;
	phase = TurnPhase::place;
	buttonVal = ButtonValues::NoButton;
	keyPressed = NO_KEY_PRESSED;
	activeTerritory = -1;
	targetTerritory = -1;
	activeTransfer = nullptr;
}

// This is for resetting variables between phases
void ResetForNextPhase(World &world, std::vector<Button> &buttons, GameState &gameState)
{
	switch (gameState.phase)
	{
	case TurnPhase::place:
	{
		gameState.transfers.clear();
		gameState.currentPlayerID = world.getNextPlayer()->getID();
		buttons.at(PhaseButton).setString("PLACE");
		buttons.at(PhaseButton).setFillColor(world.getPlayerID(gameState.currentPlayerID)->getColor());
		gameState.activeTerritory = -1;
		gameState.targetTerritory = -1;
		buttons.at(PlusButton).isActive = false;
		buttons.at(MinusButton).isActive = false;
		gameState.activeTransfer = nullptr;
		gameState.buttonVal = ButtonValues::NoButton;
		break;
	}
	case TurnPhase::attack:
	{
		buttons.at(PhaseButton).setString("ATTACK");
		gameState.activeTerritory = -1;
		gameState.targetTerritory = -1;
		buttons.at(PlusButton).isActive = false;
		buttons.at(MinusButton).isActive = false;
		gameState.buttonVal = ButtonValues::NoButton;
		break;
	}
	case TurnPhase::reposition:
	{
		buttons.at(PhaseButton).setString("REPOSITION");
		gameState.activeTerritory = -1;
		gameState.targetTerritory = -1;
		buttons.at(AttackButton).isActive = false;
		buttons.at(TextBox).isActive = false;
		gameState.buttonVal = ButtonValues::NoButton;
		break;
	}
	default:
		break;
	}
}