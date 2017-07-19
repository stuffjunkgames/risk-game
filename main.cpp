#include "risk.hpp"
#include "sfvm.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <iostream>

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1024
#define GAME_HEIGHT 768
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
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Risk");
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
    int activeTerritory = -1;

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
                    }
                }
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
            for(unsigned int i = 0; i < world.TerritoryNumber(); i++)
            {
                //window.draw(*world.getTerritory(i));
                world.getTerritory(i)->drawTerritory(&window);
                // draw text for armies
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
			clickedTerritory = getClickedTerritory(world, mousePosition);
			std::cout << "Selected Territory (index): " << clickedTerritory << std::endl;

		}

        switch(phase)
        {
        case place:
            // get number of armies and place them
            // for now: +1 army for each territory
            // later: implement bonuses

            //std::cout << (*(world.getTerritory(activeTerritory)->GetOwner()) == *currentPlayer) << std::endl;

            // active is valid and owned by current player
			if (activeTerritory >= 0 && *(world.getTerritory(activeTerritory)->GetOwner()) == *currentPlayer)
			{
                //add or remove armies
                // FIXME: suggest not using numpad.  May be fine to just add armies when you click
                if (keyPressed == 67 || keyPressed == 55) // + key
                {
                    world.getTerritory(activeTerritory)->AddArmies(1);
                    keyPressed = KEY_PRESSED_ONCE;
                }
                else if (keyPressed == 68 || keyPressed == 56) // - key
                {
                    world.getTerritory(activeTerritory)->AddArmies(-1);
                    keyPressed = KEY_PRESSED_ONCE;
                }
			}

			// clicked is valid and owned by current player
            if(clickedTerritory >= 0 && *(world.getTerritory(clickedTerritory)->GetOwner()) == *currentPlayer)
            {
				//mouse has been clicked
				if (mouseDown)
				{
					if (clickedTerritory != activeTerritory)    //clicked is different from the active
					{
						world.getTerritory(clickedTerritory)->setOutlineColor(sf::Color::Yellow);
						if (activeTerritory >= 0)   //previous is valid
                        {
                            world.getTerritory(previousTerritory)->setOutlineColor(sf::Color::Black);
                        }
                        activeTerritory = clickedTerritory;
					}
					else
					{
						if(world.getTerritory(clickedTerritory)->getOutlineColor() == sf::Color::Black)
                        {
                            world.getTerritory(clickedTerritory)->setOutlineColor(sf::Color::Yellow);
                        }
						else
                        {
                            world.getTerritory(clickedTerritory)->setOutlineColor(sf::Color::Black);
							activeTerritory = -1;
                        }

					}
					mouseDown = false;
				}
			}
			else    //either selected is invalid or it is not the current player's
			{
				if (activeTerritory >= 0 && mouseDown)//active is valid
				{
					world.getTerritory(activeTerritory)->setOutlineColor(sf::Color::Black);
					activeTerritory = -1;
					mouseDown = false;
				}
			}
            break;
        case attack:
            // attack territories


            break;
        case reposition:
            // reposition armies
            break;
        default:
            // something broke...
            break;
        }

		previousTerritory = clickedTerritory;
    }

    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////////////////


