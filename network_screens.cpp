#include "network_screens.hpp"

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1800
#define GAME_HEIGHT 1012

//sf::Font loadFont(std::string path)
//{
//	sf::Font font;
//	if (!font.loadFromFile(path))
//	{
//		// error
//		exit(EXIT_FAILURE);
//	}
//
//	return font;
//}
//
//bool loadImages(World *world)
//{
//	std::string filename{ "" };
//	std::ifstream file("borderImagesList.txt");
//
//	for (int i = 0; file >> filename; i++) {
//		world->getTerritory(i)->borderTexture = sf::Texture();
//		if (!world->getTerritory(i)->borderTexture.loadFromFile(filename))
//			return 0;
//	}
//
//	file = std::ifstream("territoryImagesList.txt");
//
//	for (int i = 0; file >> filename; i++) {
//		world->getTerritory(i)->territoryTexture = sf::Texture();
//		if (!world->getTerritory(i)->territoryTexture.loadFromFile(filename))
//			return 0;
//	}
//
//	return 1;
//}

std::string StartScreen(sf::RenderWindow &window, sf::Color* playerColor)
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
					else if (buttonExit.isInside(mousePosition) && buttonExit.isActive)
					{
						buttonPressed = BUTTON_EXIT;
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
			buttonExit.Draw(window);

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
		else if (buttonPressed == BUTTON_EXIT)
		{
			return "";
		}
	}

	return "";
}
