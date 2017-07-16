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

	world.ReadFile();

    enum TurnPhase {
        place,
        attack,
        reposition
    };

    int playerTurn = 1;
    Player* currentPlayer = world.getPlayer(0);
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

                    if(selectedTerritory >= 0)
                    {
                        world.getTerritory(selectedTerritory)->ChangeOwner(world.getPlayer(0), 5);
                    }

                }
                if(event.mouseButton.button == sf::Mouse::Right)
                {
                    // right mouse pressed

                    mousePosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

                    int tmp = getClickedTerritory(world, mousePosition);

                    if(tmp >= 0)
                    {
                        sf::Color tmpColor = world.getTerritory(tmp)->getFillColor();

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


