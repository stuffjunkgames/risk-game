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



int getClickedTerritory(World world, sf::Vector2f mousePosition)
{
    for(unsigned int i = 0; i < world.TerritoryNumber(); i++)
    {
        if(world.GetTerritory(i).isInside(mousePosition))
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

    World world;
	world.ReadFile();
    enum TurnPhase {
        place,
        attack,
        reposition
    };

    int playerTurn = 1;
    Player currentPlayer = world.GetPlayer(0);
    TurnPhase phase = place;
    int previousTerritory = -1;
    int selectedTerritory = -1;  // index of selected territory.  Negative for none selected
    bool mouseDown = false;

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
            if(event.type == sf::Event::KeyPressed)
            {

            }

            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    // left mouse pressed
                    mouseDown = true;
                    mousePosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

                    selectedTerritory = getClickedTerritory(world, mousePosition);

                    std::cout << "Selected Territory (index): " << selectedTerritory << std::endl;
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
                Territory tmp = world.GetTerritory(i);
                sf::ConvexShape s = tmp.getShape();
                world.GetTerritory(i).draw(&window);
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

        currentPlayer = world.GetPlayer(playerTurn);   // change this to base on player ID, not index

        switch(phase)
        {
        case place:
            // get number of armies and place them
            // for now: +1 army for each territory
            // later: implement bonuses

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

    }

    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////////////////


