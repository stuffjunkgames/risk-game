#include "risk.hpp"
#include <string>
#include <iostream>

class Player;
class Territory;
class ExtendedShape;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class prototypes

// ExtendedShape
ExtendedShape::ExtendedShape() : ConvexShape()
{
    // just call ConvexShape constructor
}

ExtendedShape::ExtendedShape(int n) : ConvexShape(n)
{
    // just call ConvexShape constructor
}

float ExtendedShape::Area()
{
    float A = 0;

    float xi;
    float yi;
    float xi1;
    float yi1;

    // calculate A
    for(unsigned int i = 0; i < getPointCount(); i++)
    {
        xi = getPoint(i).x;
        yi = getPoint(i).y;
        if(i < getPointCount() - 1)
        {
            xi1 = getPoint(i+1).x;
            yi1 = getPoint(i+1).y;
        }
        else
        {
            xi1 = getPoint(0).x;
            yi1 = getPoint(0).y;
        }

        A = A + (xi * yi1 - xi1*yi);
    }
    A = A / 2;

    return A;
}

sf::Vector2f ExtendedShape::Centroid()
{
    // Cx = 1/(6A)*sum((xi+x(i+1))*(xi*y(i+1)-x(i+1)*yi), i=0..n-1)
    // Cy = 1/(6A)*sum((yi+y(i+1))*(xi*y(i+1)-x(i+1)*yi), i=0..n-1)
    // A = 1/2*(sum(xi*y(i+1)-x(i+1)*yi), i=0..n-1)

    float Cx = 0;
    float Cy = 0;

    float xi;
    float yi;
    float xi1;
    float yi1;

    float A = Area();

    // calculate Cx
    for(unsigned int i = 0; i < getPointCount(); i++)
    {
        xi = getPoint(i).x;
        yi = getPoint(i).y;
        if(i < getPointCount() - 1)
        {
            xi1 = getPoint(i+1).x;
            yi1 = getPoint(i+1).y;
        }
        else
        {
            xi1 = getPoint(0).x;
            yi1 = getPoint(0).y;
        }

        Cx = Cx + ((xi + xi1) * (xi * yi1 - xi1 * yi));
    }
    Cx = Cx / (6 * A);

    // calculate Cy
    for(unsigned int i = 0; i < getPointCount(); i++)
    {
        xi = getPoint(i).x;
        yi = getPoint(i).y;
        if(i < getPointCount() - 1)
        {
            xi1 = getPoint(i+1).x;
            yi1 = getPoint(i+1).y;
        }
        else
        {
            xi1 = getPoint(0).x;
            yi1 = getPoint(0).y;
        }

        Cy = Cy + ((yi + yi1) * (xi * yi1 - xi1 * yi));
    }
    Cy = Cy / (6 * A);

    return sf::Vector2f(Cx, Cy);
}
// ExtendedShape


///////////////////////////////////////////////////////////////////////////////////////////////////
// Territory class definitions

ExtendedShape Territory::getShape()
{
    return territory;
}

bool Territory::operator == (const Territory &other)
{
    return this->id == other.id;
}

Territory::Territory(int id, ExtendedShape *shape, std::string name, Player *player, unsigned int army)
{
    this->id = id;
    territory = *shape;
    this->name = name;
    owner = player;
    this->army = army;
    territory.setFillColor(owner->color);

    // set text parameters
    if(!armyFont.loadFromFile("arial.ttf"))
    {
        // error
        exit(EXIT_FAILURE);
    }
    armyDisplay.setCharacterSize(12);
    armyDisplay.setString(std::to_string(army));
}

// if the territory object is copied, this needs to be called again
void Territory::setFont()
{
    // make sure Text's reference to Font is correct
    armyDisplay.setFont(armyFont);

    // set text position to center of shape
    sf::FloatRect bounds = armyDisplay.getGlobalBounds();
    sf::Vector2f textShift(bounds.width / 2, bounds.height / 2);
    sf::Vector2f centroid = this->territory.Centroid();

    armyDisplay.setPosition(centroid - textShift);
    armyDisplay.setFillColor(sf::Color::White);
}

// override Drawable::draw() to make this more consistent with SFML standard
// setFont MUST be called BEFORE calling draw, or drawing text will not work (probably segfault)
void Territory::draw(sf::RenderWindow *window)
{
    window->draw(territory);
    window->draw(armyDisplay);
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

bool Player::operator == (const Player &other)
{
    return this->playerNumber == other.playerNumber;
}

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

World::World()
{
    // make the world.
    // TODO: read in from a file (JSON?) or database (SQL?)

    Player player1(1, "Player 1", sf::Color::Blue);
    Player player2(2, "Player 2", sf::Color::Red);

    playerList.push_back(player1);
    playerList.push_back(player2);

    ExtendedShape shape(4);
    shape.setPointCount(4);

    shape.setPoint(0, sf::Vector2f(0,0));
    shape.setPoint(1, sf::Vector2f(0,100));
    shape.setPoint(2, sf::Vector2f(100,100));
    shape.setPoint(3, sf::Vector2f(100,0));

    territoryList.push_back(Territory(1, &shape, "Territory 1", &playerList[0], 12));

    shape.setPoint(0, sf::Vector2f(100,0));
    shape.setPoint(1, sf::Vector2f(100,100));
    shape.setPoint(2, sf::Vector2f(200,100));
    shape.setPoint(3, sf::Vector2f(200,0));

    territoryList.push_back(Territory(2, &shape, "Territory 2", &playerList[1], 5));

    shape.setPoint(0, sf::Vector2f(0,100));
    shape.setPoint(1, sf::Vector2f(0,200));
    shape.setPoint(2, sf::Vector2f(100,200));
    shape.setPoint(3, sf::Vector2f(100,100));

    territoryList.push_back(Territory(3, &shape, "Territory 3", &playerList[1], 10));

    shape.setPoint(0, sf::Vector2f(100,100));
    shape.setPoint(1, sf::Vector2f(100,200));
    shape.setPoint(2, sf::Vector2f(200,200));
    shape.setPoint(3, sf::Vector2f(200,100));

    territoryList.push_back(Territory(4, &shape, "Territory 4", &playerList[0], 20));

    for(unsigned int i = 0; i < territoryList.size(); i++)
    {
        territoryList[i].setFont();
    }

}

Territory World::getTerritory(int index)
{
    return territoryList.at(index);
}

unsigned int World::territoryNumber()
{
     return territoryList.size();
}
// World

////////////////////////////////////////////////////////////////////////////////////////////
