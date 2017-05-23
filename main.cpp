
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <iostream>

#define GAME_TOP 24
#define GAME_LEFT 12
#define GAME_WIDTH 1000
#define GAME_HEIGHT 720
#define GAME_RIGHT GAME_LEFT+GAME_WIDTH
#define GAME_BOTTOM GAME_TOP+GAME_HEIGHT

#define BLOCK_SIZE 20
#define MAX_LENGTH 100

class Player
{
    int playerNumber;
    std::string name;
    sf::Color color;
    std::list<Territory> territories; // territories owned by this player

    public Player(int number, std::string name, sf::Color color)
    {
        playerNumber = number;
        this.name = name;
        this.color = color;
    }

    void CaptureTerritory(Territory captured)
    {
        captured.ChangeOwner(this);
        territories.push_back(captured);
    }

    void LostTerritory(Territory captured)
    {
        territories.remove(captured);
    }
};

// capturable territory in game world
class Territory
{
    sf::Shape territory;
    std::string name;
    Player owner;
    unsigned int army;  // how many armies player has in territory
    std::vector<Territory> connected;

    public Territory(sf::Shape shape, std::string name, Player player, unsigned int army)
    {
        territory = shape;
        this.name = name;
        owner = player;
        this.army = army;
    }

    void addConnection(Territory connection)
    {
        connected.push_back(connection);
    }

    void ChangeOwner(Player newOwner)
    {
        owner = newOwner;
    }
};

// collection of territories that make up the game world
class World
{
    std::vector<Territory> territories;

    public World()
    {
        // make the world.
        // TODO: read in from a file (JSON?)


    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Risk");
    sf::RectangleShape background = sf::RectangleShape(sf::Vector2<float>(GAME_WIDTH, GAME_HEIGHT));
    sf::RectangleShape snake[MAX_LENGTH];
    sf::RectangleShape target = sf::RectangleShape(sf::Vector2<float>(BLOCK_SIZE, BLOCK_SIZE));

    background.setPosition(GAME_LEFT, GAME_TOP);
    background.setFillColor(sf::Color::Blue);



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
            window.display();
        }


    }

    return 0;
}
