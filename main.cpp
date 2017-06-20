#include "risk.hpp"

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

#define BLOCK_SIZE 20
#define MAX_LENGTH 100



//////////////////////////////////////////////////////////////////////////////////////////////
// main

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Risk");
    sf::RectangleShape background = sf::RectangleShape(sf::Vector2f(GAME_WIDTH, GAME_HEIGHT));
    sf::RectangleShape target = sf::RectangleShape(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));

    background.setPosition(GAME_LEFT, GAME_TOP);
    background.setFillColor(sf::Color::Green);

    World world;

    std::srand(std::time(0));

    sf::Time dt;
    sf::Time t;
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // get key presses, respond to them
            if(event.type == sf::Event::KeyPressed)
            {

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
            for(unsigned int i = 0; i < world.territoryNumber(); i++)
            {
                Territory tmp = world.getTerritory(i);
                sf::ConvexShape s = tmp.getShape();
                world.getTerritory(i).draw(&window);
                // draw text for armies
            }
            window.display();
        }


    }

    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////////////////
