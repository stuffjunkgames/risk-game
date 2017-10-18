#include "screens.hpp"

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1800
#define GAME_HEIGHT 1012
#define GAME_RIGHT GAME_LEFT+GAME_WIDTH
#define GAME_BOTTOM GAME_TOP+GAME_HEIGHT
#define NO_KEY_PRESSED -1
#define KEY_PRESSED_ONCE -2
#define NO_BUTTON_PRESSED -1
#define BUTTON_PLUS 1
#define BUTTON_MINUS 2
#define BUTTON_ATTACK 3
#define BUTTON_CHANGE_PHASE 4
#define BUTTON_TEXTBOX 5
#define BUTTON_START 6
#define BUTTON_COLORPALETTE 7
#define NUM_COLORS 4

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

std::string startScreen(sf::RenderWindow* window, sf::Color* playerColor)
{
	sf::RectangleShape background = sf::RectangleShape(sf::Vector2f(GAME_WIDTH, GAME_HEIGHT));

	sf::Texture texture;
	if (!texture.loadFromFile("title.png"))
		return "";
	sf::Sprite title(texture);
	title.setScale(1.5, 1.5);

	background.setPosition(GAME_LEFT, GAME_TOP);
	background.setFillColor(sf::Color(63, 63, 63, 255));

	sf::Font armyFont = loadFont("arial.ttf");

	Button buttonStart(armyFont, "START", sf::Vector2f(GAME_WIDTH / 2 - 137, GAME_HEIGHT / 2 - 30 + 200), 275, 60);
	buttonStart.setCharacterSize(40);
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

	unsigned int buttonPressed = -1;

	std::string playerName = "";

	bool mouseDown = false;
	int keyPressed = NO_KEY_PRESSED;

	std::srand(std::time(0));

	sf::Vector2f mousePosition;

	sf::Time dt;
	sf::Time t;
	sf::Clock clock;

	while (window->isOpen())
	{
		mouseDown = false;
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();

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
						buttonPressed = BUTTON_START;
						mouseDown = false;
					}
					else if (textBox.isInside(mousePosition) && textBox.isActive)
					{
						buttonPressed = BUTTON_TEXTBOX;
						mouseDown = false;
					}
					else if (colorPalette.isInside(mousePosition))
					{
						std::cout << std::to_string(colorPalette.getSelectedColor().r) << ", " << std::to_string(colorPalette.getSelectedColor().g) << ", " << std::to_string(colorPalette.getSelectedColor().b) << std::endl;
						buttonPressed = BUTTON_COLORPALETTE;
						mouseDown = false;
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
			window->clear();
			window->draw(background);
			window->draw(title);

			textBox.Draw(window);
			window->draw(textBoxLabel);
			buttonStart.Draw(window);

			colorPalette.Draw(window);
			window->draw(paletteLabel);

			window->display();
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
				textBox.subtractString();
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

		if (buttonPressed == BUTTON_TEXTBOX)
		{
			textBox.setOutlineColor(sf::Color::Yellow);
			isTyping = true;
		}
		else if (buttonPressed == BUTTON_START)
		{
			buttonStart.setOutlineColor(sf::Color::Yellow);
			buttonStart.Draw(window);
			window->display();
			return textBox.getLabel()->getString();
			keyPressed = NO_KEY_PRESSED;
		}
		else if (buttonPressed == BUTTON_COLORPALETTE)
		{
			playerColor->r = colorPalette.getSelectedColor().r;
			playerColor->g = colorPalette.getSelectedColor().g;
			playerColor->b = colorPalette.getSelectedColor().b;
			playerColor->a = colorPalette.getSelectedColor().a;
		}
	}

	return "";
}

int gameScreen(sf::RenderWindow* window, std::vector<std::string> playerNames, std::vector<sf::Color> playerColors)
{
	sf::RectangleShape background = sf::RectangleShape(sf::Vector2f(GAME_WIDTH, GAME_HEIGHT));

	background.setPosition(GAME_LEFT, GAME_TOP);
	background.setFillColor(sf::Color::Green);

	sf::Font armyFont = loadFont("arial.ttf");

	World world(armyFont, playerNames, playerColors);

	World initialWorld(world);

	Player* currentPlayer = world.getNextPlayer();
	TurnPhase phase = place;
	int previousTerritory = -2;
	int clickedTerritory = -1;  // index of selected territory.  Negative for none selected
	int activeTerritory = -1;// doubles as attacking territory
	int defendingTerritory = -1;
	int targetTerritory = -1;
	int armyCount = 0;
	int placedArmies = 0;
	//Arrow attackArrow;
	std::vector<Transfer> transfers;
	Transfer* activeTransfer = nullptr;
	int r;
	HoverText territoryHoverText(armyFont);
	std::vector<Label> playerLabels;
	for (unsigned int i = 0; i < world.PlayerNumber(); i++) {
		playerLabels.push_back(Label(armyFont));
		playerLabels.at(i).setFillColor(world.getPlayer(i)->getColor());
		playerLabels.at(i).setOutlineThickness(3);
		playerLabels.at(i).setCharacterSize(30);
		playerLabels.at(i).setString(world.getPlayer(i)->getName() + ":");
		playerLabels.at(i).setPosition(sf::Vector2f(GAME_WIDTH - 300, 40 * i));
	}
	Button buttonPlus(armyFont, "+", sf::Vector2f(0, 0));
	Button buttonMinus(armyFont, "-", sf::Vector2f(0, 0));
	Button buttonAttack(armyFont, "Attack", sf::Vector2f(0, 0), 80, 30);
	Button buttonPhase(armyFont, "PLACE", sf::Vector2f(0, 0), 275, 60);
	buttonPhase.setCharacterSize(40);
	buttonPhase.setFillColor(currentPlayer->getColor());
	Button buttonChangePhase(armyFont, ">", sf::Vector2f(0, 0), 60, 60);
	buttonChangePhase.setCharacterSize(40);
	buttonChangePhase.moveToPosition(sf::Vector2f(275, 0));
	TextEntry textBox(armyFont, sf::Vector2f(0, 0), 60, 30, 3);
	bool isTyping = false;
	DashedLine dLine = DashedLine();

	int buttonPressed = -1;

	sf::Texture texture;
	if (!texture.loadFromFile("new_map.png"))
		return EXIT_FAILURE;
	sf::Sprite map(texture);
	map.setScale(1.5, 1.5);

	sf::Texture greyTexture;
	if (!greyTexture.loadFromFile("grey.png"))
		return EXIT_FAILURE;
	sf::Sprite greySprite(greyTexture);
	greySprite.setScale(1.5, 1.5);

	sf::Texture normalBordersTexture;
	if (!normalBordersTexture.loadFromFile("normal_borders.png"))
		return EXIT_FAILURE;
	sf::Sprite normalBordersSprite(normalBordersTexture);
	normalBordersSprite.setScale(1.5, 1.5);

	// TODO: code to load each of the border images
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
	}

	bool mouseDown = false;
	int keyPressed = NO_KEY_PRESSED;

	std::srand(std::time(0));

	sf::Vector2f mousePosition;
	sf::Vector2f mouseHover;

	sf::Time dt;
	sf::Time t;
	sf::Clock clock;

	while (window->isOpen())
	{
		mouseDown = false;
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();

			// get key presses, respond to them
			if (event.type == sf::Event::KeyPressed)
			{
				//this is only working for one key pressed at a time
				if (keyPressed == NO_KEY_PRESSED)
				{
					keyPressed = event.key.code;
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

					// Set flags for button presses
					if (buttonPlus.isInside(mousePosition) && buttonPlus.isActive)
					{
						buttonPressed = BUTTON_PLUS;
						mouseDown = false;
					}
					else if (buttonMinus.isInside(mousePosition) && buttonMinus.isActive)
					{
						buttonPressed = BUTTON_MINUS;
						mouseDown = false;
					}
					else if (buttonAttack.isInside(mousePosition) && buttonAttack.isActive)
					{
						buttonPressed = BUTTON_ATTACK;
						mouseDown = false;
					}
					else if (buttonChangePhase.isInside(mousePosition) && buttonChangePhase.isActive)
					{
						buttonPressed = BUTTON_CHANGE_PHASE;
						mouseDown = false;
					}
					else if (textBox.isInside(mousePosition) && textBox.isActive)
					{
						buttonPressed = BUTTON_TEXTBOX;
						mouseDown = false;
					}
					else
					{
						isTyping = false;
						textBox.setIsTyping(isTyping);
						buttonPressed = NO_BUTTON_PRESSED;
					}
				}
			}
			if (event.type == sf::Event::MouseMoved)
			{
				mouseHover = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
			}
		}

		// draw
		dt = clock.restart();
		t += dt;
		if (t.asMilliseconds() >= 16)
		{
			t = sf::Time::Zero;

			window->clear();
			window->draw(background);
			window->draw(map);

			for (unsigned int i = 0; i < world.TerritoryNumber(); i++)
			{
				world.getTerritory(i)->drawTerritory(window);
			}
			window->draw(normalBordersSprite);

			if (activeTerritory >= 0 && activeTerritory < (int)world.TerritoryNumber()) {
				window->draw(world.getTerritory(activeTerritory)->borderSprite);
			}

			if (phase == place)
			{
				if (activeTerritory >= 0)
				{
					buttonPlus.Draw(window);
					buttonMinus.Draw(window);
				}
				else
				{
					buttonPlus.isActive = false;
					buttonMinus.isActive = false;
				}
			}
			else if (phase == attack)
			{
				if (defendingTerritory >= 0)
				{
					dLine.Draw(window, world.getTerritory(previousTerritory)->centerPos, world.getTerritory(defendingTerritory)->centerPos);
					buttonAttack.Draw(window);
					textBox.Draw(window);
				}
				else
				{
					buttonAttack.isActive = false;
					textBox.isActive = false;
				}
				if (activeTerritory >= 0)
				{
					window->draw(greySprite);
					for (unsigned int i = 0; i < world.getTerritory(activeTerritory)->getConnected()->size(); i++)
					{
						if (world.getTerritory(activeTerritory)->getConnected()->at(i)->GetOwner() != world.getTerritory(activeTerritory)->GetOwner())
						{
							world.getTerritory(activeTerritory)->getConnected()->at(i)->drawTerritory(window);
						}
					}
					world.getTerritory(activeTerritory)->drawTerritory(window);
					window->draw(normalBordersSprite);
					dLine.Draw(window, world.getTerritory(activeTerritory)->centerPos, mouseHover);
				}
			}
			else if (phase == reposition)
			{
				if (targetTerritory >= 0)
				{
					dLine.Draw(window, world.getTerritory(previousTerritory)->centerPos, world.getTerritory(targetTerritory)->centerPos);
					buttonPlus.Draw(window);
					buttonMinus.Draw(window);
				}
				else if (activeTerritory >= 0)
				{
					window->draw(greySprite);
					for (unsigned int i = 0; i < world.getTerritory(activeTerritory)->getConnected()->size(); i++)
					{
						if (world.getTerritory(activeTerritory)->getConnected()->at(i)->GetOwner() == world.getTerritory(activeTerritory)->GetOwner())
						{
							world.getTerritory(activeTerritory)->getConnected()->at(i)->drawTerritory(window);
						}
					}
					world.getTerritory(activeTerritory)->drawTerritory(window);
					window->draw(normalBordersSprite);
					dLine.Draw(window, world.getTerritory(activeTerritory)->centerPos, mouseHover);
					buttonPlus.isActive = false;
					buttonMinus.isActive = false;
				}
				else
				{
					buttonPlus.isActive = false;
					buttonMinus.isActive = false;
				}
				for (Transfer t : transfers)
				{
					t.Draw(window);
				}
			}

			for (unsigned int i = 0; i < playerLabels.size(); i++)
			{
				playerLabels.at(i).setString(world.getPlayer(i)->getName() + ": " + std::to_string(3 + world.GetBonus(i)));
				window->draw(playerLabels.at(i));
			}
			buttonPhase.Draw(window);
			buttonChangePhase.Draw(window);

			sf::Color pxColor;
			for (unsigned int i = 0; i < world.TerritoryNumber(); i++)
			{
				pxColor = world.getTerritory(i)->territoryImage.getPixel(mouseHover.x / 1.5, mouseHover.y / 1.5); // devision by 1.5 because everything is scaled up by 1.5
				if (pxColor.a > 0) {
					territoryHoverText.setText(world.getTerritory(i)->getName() + " " + std::to_string(i) +
						"\n" + world.GetBonusName(i) + " " + std::to_string(world.GetBonusIncome(i)),
						mouseHover.x, mouseHover.y);
					territoryHoverText.Draw(window);
					break;
				}
			}

			window->display();
		}

		// game logic
		// thread this eventually
		// get mouse click above
		// check if click is inside territory
		// keep track of previous clicks inside territories
		// make moves based on clicks
		// turn has 3 parts:  place units -> attack -> reposition

		if (mouseDown)
		{
			previousTerritory = clickedTerritory;

			// Check the territory images for non-zero alpha value at mousePosition
			sf::Color pxColor;
			clickedTerritory = -1;
			for (unsigned int i = 0; i < world.TerritoryNumber() && clickedTerritory == -1; i++) {
				pxColor = world.getTerritory(i)->territoryImage.getPixel(mousePosition.x / 1.5, mousePosition.y / 1.5); // devision by 1.5 because everything is scaled up by 1.5
				if (pxColor.a > 0) {
					clickedTerritory = i;
				}
				else {
					clickedTerritory = -1;
				}
			}
			if (clickedTerritory >= 0 && *(world.getTerritory(clickedTerritory)->GetOwner()) == *currentPlayer)
			{
				if (clickedTerritory != activeTerritory)
				{
					activeTerritory = clickedTerritory;
				}
				else
				{
					activeTerritory = -1;
				}
			}
			else
			{
				activeTerritory = -1;
			}
		}

		if (isTyping)
		{
			if (keyPressed >= sf::Keyboard::Num0 && keyPressed <= sf::Keyboard::Num9)
			{
				textBox.appendString(std::to_string(keyPressed - sf::Keyboard::Num0));

				keyPressed = KEY_PRESSED_ONCE;
			}
			else if (keyPressed >= sf::Keyboard::Numpad0 && keyPressed <= sf::Keyboard::Numpad9)
			{
				textBox.appendString(std::to_string(keyPressed - sf::Keyboard::Numpad0));

				keyPressed = KEY_PRESSED_ONCE;
			}
			else if (keyPressed == sf::Keyboard::BackSpace)
			{
				textBox.subtractString();

				keyPressed = KEY_PRESSED_ONCE;
			}
		}

		switch (phase)
		{
		case place:
		{
			// get number of armies and place them
			// for now: +1 army for each territory
			// later: implement bonuses

			if (currentPlayer->getNumTerritories() <= 0)
			{
				currentPlayer = world.getNextPlayer();
				break;
			}

			// first time through, count the number of armies player gets
			if (armyCount == 0)
			{
				// FIXME: get rid of these "magic numbers"
				//armyCount = 3 + currentPlayer->getNumTerritories();
				initialWorld = world;
				armyCount = 3 + world.GetBonus(currentPlayer->getID());
				placedArmies = 0;
				std::cout << "Placeable count: " << armyCount << std::endl;
			}

			// if they are all placed, move on.
			if (placedArmies == armyCount)
			{
				armyCount = 0;
				if (activeTerritory >= 0)
				{
					// no more active territory
					world.getTerritory(activeTerritory)->setOutlineColor(sf::Color::Black);
					std::cout << "changing phase" << std::endl;
					activeTerritory = -1;
				}
				phase = attack;
				buttonPhase.setString("ATTACK");
				clickedTerritory = -1;
				previousTerritory = -1;
				buttonPlus.isActive = false;
				buttonMinus.isActive = false;
				buttonPressed = NO_BUTTON_PRESSED;
				break;
			}

			// active is valid and owned by current player
			if (activeTerritory >= 0 && *(world.getTerritory(activeTerritory)->GetOwner()) == *currentPlayer)
			{
				buttonPlus.moveToPosition(world.getTerritory(activeTerritory)->centerPos + sf::Vector2f(30, -25));
				buttonMinus.moveToPosition(world.getTerritory(activeTerritory)->centerPos + sf::Vector2f(30, 7));
				//add or remove armies
				// FIXME: suggest not using numpad.  May be fine to just add armies when you click
				// FIXME: maybe add/remove to/from a buffer and apply when changing phase, so the territories aren't directly altered
				if (keyPressed == sf::Keyboard::Add || keyPressed == sf::Keyboard::Equal || buttonPressed == BUTTON_PLUS) // + key
				{
					placedArmies += Place(&world, &initialWorld, currentPlayer, activeTerritory, 1, armyCount - placedArmies);
					//world.getTerritory(activeTerritory)->AddArmies(1);
					if (buttonPressed == BUTTON_PLUS)
					{
						keyPressed = NO_KEY_PRESSED;
					}
					else
					{
						keyPressed = KEY_PRESSED_ONCE;
					}
					buttonPressed = NO_BUTTON_PRESSED;
					//placedArmies = placedArmies + 1;
				}
				else if (keyPressed == sf::Keyboard::Subtract || keyPressed == sf::Keyboard::Dash || buttonPressed == BUTTON_MINUS) // - key
				{
					placedArmies += Place(&world, &initialWorld, currentPlayer, activeTerritory, -1, armyCount - placedArmies);

					if (buttonPressed == BUTTON_MINUS)
					{
						keyPressed = NO_KEY_PRESSED;
					}
					else
					{
						keyPressed = KEY_PRESSED_ONCE;
					}

					//                        if(world.getTerritory(activeTerritory)->GetArmies() > initialWorld.getTerritory(activeTerritory)->GetArmies())
					//						{
					//						    world.getTerritory(activeTerritory)->AddArmies(-1);
					//                            placedArmies = placedArmies - 1;
					//						}

					buttonPressed = NO_BUTTON_PRESSED;
				}
			}
			break;
		}
		case attack:
		{
			// attack territories

			if (mouseDown && clickedTerritory >= 0 && previousTerritory >= 0)
			{
				if (*(world.getTerritory(previousTerritory)->GetOwner()) == *currentPlayer &&
					!(*(world.getTerritory(clickedTerritory)->GetOwner()) == *currentPlayer) &&
					world.getTerritory(clickedTerritory)->isConnected(world.getTerritory(previousTerritory)))
				{
					defendingTerritory = clickedTerritory;
					buttonAttack.moveToPosition(dLine.getCenter());
					textBox.moveToPosition(sf::Vector2f(buttonAttack.getLocalBounds().left + buttonAttack.getLocalBounds().width, buttonAttack.getLocalBounds().top));
				}
				else
				{
					defendingTerritory = -1;
				}
			}
			else if (mouseDown)
			{
				defendingTerritory = -1;
			}
			if (defendingTerritory >= 0)
			{
				if ((keyPressed == sf::Keyboard::Space || buttonPressed == BUTTON_ATTACK) && world.getTerritory(previousTerritory)->GetArmies() > 1) // spacebar key
				{

					Attack(&world, currentPlayer, previousTerritory, defendingTerritory, 1);

					// FIXME: code to determine the winner
					//                            r = rand();
					//                            if (r > RAND_MAX / 2)
					//                            {
					//                                // attacking player wins
					//
					//                                WORLD.GETTERRITORY(DEFENDINGTERRITORY)->ADDARMIES(-1);
					//                                IF(WORLD.GETTERRITORY(DEFENDINGTERRITORY)->GETARMIES() <= 0)
					//                                {
					//                                    CURRENTPLAYER->CAPTURETERRITORY(WORLD.GETTERRITORY(DEFENDINGTERRITORY), 1);
					//                                    WORLD.GETTERRITORY(PREVIOUSTERRITORY)->ADDARMIES(-1);
					//                                    DEFENDINGTERRITORY = -1;
					//                                }
					//                            }
					//                            else
					//                            {
					//                                // defending player wins
					//                                world.getTerritory(previousTerritory)->AddArmies(-1);
					//                            }

					if (buttonPressed == BUTTON_ATTACK)
					{
						keyPressed = NO_KEY_PRESSED;
					}
					else
					{
						keyPressed = KEY_PRESSED_ONCE;
					}
					buttonPressed = NO_BUTTON_PRESSED;
				}
				else if (world.getTerritory(previousTerritory)->GetArmies() <= 1)
				{
					defendingTerritory = -1;
				}
				else if (buttonPressed == BUTTON_TEXTBOX)
				{
					isTyping = true;
					textBox.setIsTyping(isTyping);
				}
			}

			if (keyPressed == sf::Keyboard::Return || buttonPressed == BUTTON_CHANGE_PHASE)
			{
				std::cout << "Ending attack phase and entering reposition" << std::endl;
				phase = reposition;
				buttonPhase.setString("REPOSITION");
				defendingTerritory = -1;
				initialWorld = world;
				targetTerritory = -1;
				activeTerritory = -1;
				clickedTerritory = -1;
				previousTerritory = -1;
				buttonAttack.isActive = false;
				textBox.isActive = false;
				buttonPressed = NO_BUTTON_PRESSED;
			}

			break;
		}
		case reposition:
		{
			// reposition armies

			// need to track which armies have moved so they can't move again in this phase

			// make copy of world object for initial states (maybe just copy territory vector)
			// When unit moves:
			// move unit in real world
			// remove unit from initial state copy
			// if initial state is at 1, can't move any more from that territory

			// enter key exits reposition phase and starts next player's turn

			// track which armies have moved


			// valid territories have been clicked on... Draw arrows
			if (mouseDown && clickedTerritory >= 0 && previousTerritory >= 0)
			{
				if (*(world.getTerritory(previousTerritory)->GetOwner()) == *currentPlayer &&
					*(world.getTerritory(clickedTerritory)->GetOwner()) == *currentPlayer &&
					world.getTerritory(clickedTerritory)->isConnected(world.getTerritory(previousTerritory)))
				{
					for (std::vector<Transfer>::iterator it = transfers.begin(); it != transfers.end(); it++)
					{
						if (it->getAmount() <= 0)
						{
							transfers.erase(it);
						}
					}
					for (unsigned int i = 0; i < transfers.size(); i++)
					{
						if (transfers.at(i).donor == previousTerritory && transfers.at(i).receiver == clickedTerritory) {
							activeTransfer = &transfers.at(i);

							Arrow tmpArrow = Arrow(world.getTerritory(previousTerritory)->centerPos, world.getTerritory(clickedTerritory)->centerPos);
							buttonPlus.moveToPosition(tmpArrow.Centroid() + sf::Vector2f(30, -25));
							buttonMinus.moveToPosition(tmpArrow.Centroid() + sf::Vector2f(30, 7));

							std::cout << "found a match, from " << activeTransfer->donor << " to " << activeTransfer->receiver << std::endl;
							break;
						}
						else
						{
							activeTransfer = nullptr;
						}
					}
					if (activeTransfer == nullptr) {
						buttonPlus.moveToPosition(dLine.getCenter() + sf::Vector2f(30, -25));
						buttonMinus.moveToPosition(dLine.getCenter() + sf::Vector2f(30, 7));
						transfers.push_back(Transfer(armyFont, previousTerritory, clickedTerritory, dLine));
						transfers.back().setLinePoints(world.getTerritory(previousTerritory)->centerPos, world.getTerritory(activeTerritory)->centerPos);
						activeTransfer = &transfers.back();
					}

					targetTerritory = clickedTerritory;
					activeTerritory = -1;
					clickedTerritory = -1;
				}
				else
				{
					targetTerritory = -1;
					activeTransfer = nullptr;
				}
			}
			else if (mouseDown)  // invalid territory.  Clear target
			{
				std::vector<Transfer>::iterator it = transfers.begin();
				while (transfers.size() > 0 && it != transfers.end())
				{
					if (it->getAmount() <= 0)
					{
						transfers.erase(it);
					}
					else
					{
						it++;
					}
				}
				activeTransfer = nullptr;
				targetTerritory = -1;
			}

			if ((keyPressed == sf::Keyboard::Add || keyPressed == sf::Keyboard::Equal || buttonPressed == BUTTON_PLUS) // + pressed
				&& targetTerritory >= 0 // target territory exists
				&& initialWorld.getTerritory(previousTerritory)->GetArmies() > 1)  // source territory has unmoved armies
			{
				initialWorld.getTerritory(previousTerritory)->AddArmies(-1);    // remove army from initial distribution
				activeTransfer->increaseAmount(1);
				if (buttonPressed == BUTTON_PLUS)
				{
					keyPressed = NO_KEY_PRESSED;
				}
				else
				{
					keyPressed = KEY_PRESSED_ONCE;
				}
				buttonPressed = NO_BUTTON_PRESSED;
			}
			if ((keyPressed == sf::Keyboard::Dash || keyPressed == sf::Keyboard::Subtract || buttonPressed == BUTTON_MINUS) // + pressed
				&& targetTerritory >= 0 // target territory exists
				&& activeTransfer->getAmount() > 0)  // a transfer has been commanded
			{
				initialWorld.getTerritory(previousTerritory)->AddArmies(1);    // remove army from initial distribution
				activeTransfer->increaseAmount(-1);
				if (buttonPressed == BUTTON_MINUS)
				{
					keyPressed = NO_KEY_PRESSED;
				}
				else
				{
					keyPressed = KEY_PRESSED_ONCE;
				}
				buttonPressed = NO_BUTTON_PRESSED;
			}
			else if (keyPressed == sf::Keyboard::Return || buttonPressed == BUTTON_CHANGE_PHASE)
			{
				for (Transfer t : transfers)
				{
					world.getTerritory(t.donor)->AddArmies(-t.getAmount());		// remove army from source
					world.getTerritory(t.receiver)->AddArmies(t.getAmount());	// add army to target
				}
				transfers.clear();
				currentPlayer = world.getNextPlayer();
				phase = place;
				buttonPhase.setString("PLACE");
				buttonPhase.setFillColor(currentPlayer->getColor());
				clickedTerritory = -1;
				activeTerritory = -1;
				previousTerritory = -1;
				buttonPlus.isActive = false;
				buttonMinus.isActive = false;
				activeTransfer = nullptr;
				buttonPressed = NO_BUTTON_PRESSED;
			}

			break;
		}
		default:
		{
			// something broke...
			break;
		}
		}

		mouseDown = false;
	}

	return 0;
}
