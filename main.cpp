#include "risk.hpp"
#include "sfvm.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1600
#define GAME_HEIGHT 1200
#define GAME_RIGHT GAME_LEFT+GAME_WIDTH
#define GAME_BOTTOM GAME_TOP+GAME_HEIGHT
#define NO_KEY_PRESSED -1
#define KEY_PRESSED_ONCE -2


sf::Font loadFont(std::string path)
{
    sf::Font font;
    if(!font.loadFromFile(path))
    {
        // error
        exit(EXIT_FAILURE);
    }

    return font;
}

int getClickedTerritory(World world, sf::Vector2f mousePosition)
{
    for(unsigned int i = 0; i < world.TerritoryNumber(); i++)
    {
        if(world.getTerritory(i)->isInside(mousePosition))
        {
            return i;
        }
    }

    return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// main

int main()
{
    sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Risk");
    sf::RectangleShape background = sf::RectangleShape(sf::Vector2f(GAME_WIDTH, GAME_HEIGHT));

    background.setPosition(GAME_LEFT, GAME_TOP);
    background.setFillColor(sf::Color::Green);


    sf::Font armyFont = loadFont("arial.ttf");

    World world(armyFont);

	//world.ReadFile();

    enum TurnPhase {
        place,
        attack,
        reposition
    };

    int playerTurn = 0;
    Player* currentPlayer = world.getPlayer(playerTurn);
    TurnPhase phase = place;
    int previousTerritory = -2;
    int clickedTerritory = -1;  // index of selected territory.  Negative for none selected
    int activeTerritory = -1;// doubles as attacking territory
	int defendingTerritory = -1;
    int armyCount = 0;
    int placedArmies = 0;
	Arrow attackArrow;
	int r;

	sf::Texture texture;
	if (!texture.loadFromFile("map.jpg"))
		return EXIT_FAILURE;
	sf::Sprite map(texture);
	map.setScale(1.5, 1.5);

	/////////////////////////////////////////////
	// Only for making better map, delete after

	std::ofstream myfile;
	std::string line;
	std::string chunk;
	sf::Vector2f pos(0, 0);

	myfile.open("world2.txt");

	/////////////////////////////////////////////

    bool mouseDown = false;
	int keyPressed = -1;

    std::srand(std::time(0));

    sf::Vector2f mousePosition;

    sf::Time dt;
    sf::Time t;
    sf::Clock clock;

    while (window.isOpen())
    {
        mouseDown = false;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

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

            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    // left mouse pressed
                    mouseDown = true;
                    mousePosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                }
                if(event.mouseButton.button == sf::Mouse::Right)
                {
                    // right mouse pressed

                    mousePosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

                    int tmp = getClickedTerritory(world, mousePosition);

                    if(tmp >= 0)
                    {
                        sf::Color tmpColor = world.getTerritory(tmp)->getFillColor();
						//world.getTerritory(tmp)->ChangeOwner(world.getPlayer(1), 5);
                        std::cout << (int)tmpColor.r << ", " << (int)tmpColor.g << ", " << (int)tmpColor.b << std::endl;
						std::cout << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
                    }
                }
            }
			if (event.type == sf::Event::MouseMoved)
			{
				pos.x = event.mouseMove.x;
				pos.y = event.mouseMove.y;
			}
        }

        // draw
        dt = clock.restart();
        t += dt;
        if(t.asMilliseconds() >= 100){

            t = sf::Time::Zero;

            // draw
            window.clear();
            window.draw(background);
			window.draw(map);
            for(unsigned int i = 0; i < world.TerritoryNumber(); i++)
            {
                //window.draw(*world.getTerritory(i));
                world.getTerritory(i)->drawTerritory(&window);
                // draw text for armies
            }
			if (defendingTerritory >= 0)
			{
				attackArrow.Draw(&window);
			}
            window.display();
        }

        // game logic
        // thread this eventually
        // get mouse click above
        // check if click is inside territory
        // keep track of previous clicks inside territories
        // make moves based on clicks
        // turn has 3 parts:  place units -> attack -> relocate

        currentPlayer = world.getPlayer(playerTurn);   // change this to base on player ID, not index

		if (mouseDown)
		{
			previousTerritory = clickedTerritory;
			clickedTerritory = getClickedTerritory(world, mousePosition);
			std::cout << "Selected Territory (index): " << clickedTerritory << std::endl;

		}
		// clicked is valid and owned by current player
		if (clickedTerritory >= 0 && *(world.getTerritory(clickedTerritory)->GetOwner()) == *currentPlayer)
		{
			//mouse has been clicked
			if (mouseDown)
			{
				if (clickedTerritory != activeTerritory)    //clicked is different from the active
				{
					world.getTerritory(clickedTerritory)->setOutlineColor(sf::Color::Yellow);
					if (activeTerritory >= 0)   //active is valid
					{
						world.getTerritory(previousTerritory)->setOutlineColor(sf::Color::Black);
					}
					activeTerritory = clickedTerritory;
				}
				else
				{
					if (world.getTerritory(clickedTerritory)->getOutlineColor() == sf::Color::Black)
					{
						world.getTerritory(clickedTerritory)->setOutlineColor(sf::Color::Yellow);
					}
					else
					{
						world.getTerritory(clickedTerritory)->setOutlineColor(sf::Color::Black);
						activeTerritory = -1;
					}

				}
				//mouseDown = false;
			}
		}
		else    //either selected is invalid or it is not the current player's
		{
			if (activeTerritory >= 0 && mouseDown)//active is valid
			{
				world.getTerritory(activeTerritory)->setOutlineColor(sf::Color::Black);
				activeTerritory = -1;
				//mouseDown = false;
			}
		}

        switch(phase)
        {
        case place:
            // get number of armies and place them
            // for now: +1 army for each territory
            // later: implement bonuses

            // first time through, count the number of armies player gets
            if(armyCount == 0)
            {
                // FIXME: get rid of these "magic numbers"
                armyCount = 3 + currentPlayer->getNumTerritories();
                placedArmies = 0;
                std::cout << currentPlayer->getNumTerritories() << std::endl;
            }

            // if they are all placed, move on.
            if(placedArmies == armyCount)
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
                break;
            }

            // active is valid and owned by current player
			if (activeTerritory >= 0 && *(world.getTerritory(activeTerritory)->GetOwner()) == *currentPlayer)
			{
                //add or remove armies
                // FIXME: suggest not using numpad.  May be fine to just add armies when you click
				// FIXME: maybe add/remove to/from a buffer and apply when changing phase, so the territories aren't directly altered
                if (keyPressed == 67 || keyPressed == 55) // + key
                {
                    world.getTerritory(activeTerritory)->AddArmies(1);
                    keyPressed = KEY_PRESSED_ONCE;
                    placedArmies = placedArmies + 1;
                }
                else if (keyPressed == 68 || keyPressed == 56) // - key
                {
                    world.getTerritory(activeTerritory)->AddArmies(-1);
                    keyPressed = KEY_PRESSED_ONCE;
                    placedArmies = placedArmies - 1;
                }
			}
            break;
        case attack:
            // attack territories

                if (mouseDown && clickedTerritory >= 0 && previousTerritory >= 0)
                {
                    if (*(world.getTerritory(previousTerritory)->GetOwner()) == *currentPlayer &&
                        !(*(world.getTerritory(clickedTerritory)->GetOwner()) == *currentPlayer) &&
                        world.getTerritory(clickedTerritory)->isConnected(world.getTerritory(previousTerritory)))
                    {
                        defendingTerritory = clickedTerritory;
                        attackArrow = Arrow(world.getTerritory(previousTerritory)->Centroid(), world.getTerritory(clickedTerritory)->Centroid());
                    }
                }
                if (defendingTerritory >= 0)
                {
                    if (keyPressed == 57 && world.getTerritory(previousTerritory)->GetArmies() > 1) // spacebar key
                    {
                        // FIXME: code to determine the winner
                        r = rand();
                        if (r > RAND_MAX / 2)
                        {
                            // attacking player wins

                            world.getTerritory(defendingTerritory)->AddArmies(-1);
                            if(world.getTerritory(defendingTerritory)->GetArmies() <= 0)
                            {
                                currentPlayer->CaptureTerritory(world.getTerritory(defendingTerritory), 1);
                                world.getTerritory(previousTerritory)->AddArmies(-1);
                            }
                        }
                        else
                        {
                            // defending player wins
                            world.getTerritory(previousTerritory)->AddArmies(-1);
                        }

                        keyPressed = KEY_PRESSED_ONCE;
                        defendingTerritory = -1;
                    }
                }

            break;
        case reposition:
            // reposition armies
            break;
        default:
            // something broke...
            break;
        }

		if (keyPressed == 57) // spacebar key
		{
			myfile << pos.x << " " << pos.y << "|";
			keyPressed = KEY_PRESSED_ONCE;
		}
		if (keyPressed == 58)
		{
			myfile << std::endl;
			keyPressed = KEY_PRESSED_ONCE;
		}

		mouseDown = false;
    }
	myfile.close();
    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////////////////

