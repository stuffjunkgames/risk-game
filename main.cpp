
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <iostream>
#include <list>

#define GAME_TOP 24
#define GAME_LEFT 12
#define GAME_WIDTH 1000
#define GAME_HEIGHT 720
#define GAME_RIGHT GAME_LEFT+GAME_WIDTH
#define GAME_BOTTOM GAME_TOP+GAME_HEIGHT

#define BLOCK_SIZE 20
#define MAX_LENGTH 100

class Player;
class Territory;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class prototypes

// capturable territory in game world
class Territory
{
    int id;
    sf::ConvexShape territory;
    sf::Text armyDisplay;
    std::string name;
    Player *owner;
    unsigned int army;  // how many armies player has in territory
    std::vector<Territory> connected;

public:
    Territory(int id, sf::ConvexShape *shape, std::string name, Player *player, unsigned int army);
    void addConnection(Territory connection);
    void ChangeOwner(Player *newOwner, unsigned int newArmy);
    sf::ConvexShape getShape()
    {
        return territory;
    }

    bool operator == (const Territory &other)
    {
        return this->id == other.id;
    }

    // find centroid of territory
    // move armyDisplay to centroid of territory
    // set armyDisplay number
    // add function for adding/subtracting armies

}; // Territory

class Player
{
    int playerNumber;
    std::string name;
    std::list<Territory> territories; // territories owned by this player

public:
    sf::Color color;

    Player(int number, std::string name, sf::Color color);
    void CaptureTerritory(Territory *captured, unsigned int army);
    void LostTerritory(Territory *captured);

    bool operator == (const Player &other)
    {
        return this->playerNumber == other.playerNumber;
    }

}; // Player

// Class prototypes ^
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Territory class definitions

Territory::Territory(int id, sf::ConvexShape *shape, std::string name, Player *player, unsigned int army)
{
    this->id = id;
    territory = *shape;
    this->name = name;
    owner = player;
    this->army = army;
    territory.setFillColor(owner->color);
}

void Territory::addConnection(Territory connection)
{
    connected.push_back(connection);
}

void Territory::ChangeOwner(Player *newOwner, unsigned int newArmy)
{
    owner = newOwner;
    army = newArmy;
    territory.setFillColor(owner->color);
}

// Territory class ^
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Player class definitions

Player::Player(int number, std::string name, sf::Color color)
{
    playerNumber = number;
    this->name = name;
    this->color = color;
}


void Player::CaptureTerritory(Territory *captured, unsigned int army)
{
    captured->ChangeOwner(this, army);
    territories.push_back(*captured);
}

void Player::LostTerritory(Territory *captured)
{
    territories.remove(*captured);
}

// Player class definitions ^
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// World class

// collection of territories that make up the game world
class World
{
    std::vector<Territory> territoryList;
    std::vector<Player> playerList;

public:
    World()
    {
        // make the world.
        // TODO: read in from a file (JSON?) or database (SQL?)

        playerList.push_back(*(new Player(1, "Player 1", sf::Color::Blue)));
        playerList.push_back(*(new Player(2, "Player 2", sf::Color::Red)));

        sf::ConvexShape shape(4);
        shape.setPointCount(4);

        shape.setPoint(0, sf::Vector2f(0,0));
        shape.setPoint(1, sf::Vector2f(0,100));
        shape.setPoint(2, sf::Vector2f(100,100));
        shape.setPoint(3, sf::Vector2f(100,0));

        territoryList.push_back(*(new Territory(1, &shape, "Territory 1", &playerList[0], 12)));

        shape.setPoint(0, sf::Vector2f(100,0));
        shape.setPoint(1, sf::Vector2f(100,100));
        shape.setPoint(2, sf::Vector2f(200,100));
        shape.setPoint(3, sf::Vector2f(200,0));

        territoryList.push_back(*(new Territory(1, &shape, "Territory 2", &playerList[1], 5)));

        shape.setPoint(0, sf::Vector2f(0,100));
        shape.setPoint(1, sf::Vector2f(0,200));
        shape.setPoint(2, sf::Vector2f(100,200));
        shape.setPoint(3, sf::Vector2f(100,100));

        territoryList.push_back(*(new Territory(1, &shape, "Territory 3", &playerList[1], 10)));

        shape.setPoint(0, sf::Vector2f(100,100));
        shape.setPoint(1, sf::Vector2f(100,200));
        shape.setPoint(2, sf::Vector2f(200,200));
        shape.setPoint(3, sf::Vector2f(200,100));

        territoryList.push_back(*(new Territory(1, &shape, "Territory 4", &playerList[0], 20)));

    }

    Territory getTerritory(int index)
    {
        return territoryList.at(index);
    }

    unsigned int territoryNumber()
    {
         return territoryList.size();
    }
}; // World

////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// main

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Risk");
    sf::RectangleShape background = sf::RectangleShape(sf::Vector2<float>(GAME_WIDTH, GAME_HEIGHT));
    sf::RectangleShape target = sf::RectangleShape(sf::Vector2<float>(BLOCK_SIZE, BLOCK_SIZE));

    background.setPosition(GAME_LEFT, GAME_TOP);
    background.setFillColor(sf::Color::Black);

    World world = *(new World());

    sf::ConvexShape convex;

    convex.setPointCount(5);

    convex.setPoint(0, sf::Vector2f(0, 0));
    convex.setPoint(1, sf::Vector2f(150, 10));
    convex.setPoint(2, sf::Vector2f(120, 90));
    convex.setPoint(3, sf::Vector2f(30, 100));
    convex.setPoint(4, sf::Vector2f(0, 50));

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
            window.draw(convex);
            for(unsigned int i = 0; i < world.territoryNumber(); i++)
            {
                Territory tmp = world.getTerritory(i);
                sf::ConvexShape s = tmp.getShape();
                window.draw(world.getTerritory(i).getShape());
                // draw text for armies
            }
            window.display();
        }


    }

    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////////////////
