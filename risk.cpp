#include "risk.hpp"
#include "sfvm.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

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

bool ExtendedShape::isInside(sf::Vector2f point)
{
    // for each point p in shape
    // calculate cross product of <p, p+1> and <p, point>
    // get z component
    // if z has different sign from previous z, point is not inside shape

    // initialize loop with first cross product
    sf::Vector2f u = getPoint(1) - getPoint(0);
    sf::Vector2f v = point - getPoint(0);

    sf::Vector3f last = sfvm::Cross(u,v);
    sf::Vector3f next;

    for(unsigned int i = 1; i < getPointCount() - 1; i++)
    {
        u = getPoint(i + 1) - getPoint(i);
        v = point - getPoint(i);

        next = sfvm::Cross(u,v);

        if(!(last.z > 0 && next.z > 0) && !(last.z < 0 && next.z < 0))
        {
            return false;
        }

        last = next;
    }

    // also need to check vector from last point to first point
    u = getPoint(0) - getPoint(getPointCount() - 1);
    v = point - getPoint(getPointCount() - 1);

    next = sfvm::Cross(u,v);

    if(!(last.z > 0 && next.z > 0) && !(last.z < 0 && next.z < 0))
    {
        return false;
    }

    return true;
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

// returns whether point is inside the territory
bool Territory::isInside(sf::Vector2f point)
{
    // just call ExtendedShape.isInside
    return territory.isInside(point);
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

void World::ReadFile()
{
	std::ifstream infile("world.txt");
	std::string line;
	std::string chunk;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		//std::cout << line << std::endl;

		//parsing
		chunk = "";
		char section = 0;// 0:coordinates, 1:name, 2:id, 3:armysize
		int x, y; //coordinates
		std::vector<std::vector<int> > xyPairs;
		std::string name;
		int id;
		int armies;
		for (char c : line)
		{
			switch(section)
			{
			case 0:
				if (c == ' ') {
					x = std::stoi(chunk);
					chunk = "";
					break;
				}
				else if (c == '|' || c == ';') {
					y = std::stoi(chunk);
					// do something with the x and y coordinates
					xyPairs.push_back({ x, y });
					chunk = "";
					if (c == ';')
						section = 1;
					break;
				}
				chunk += c;
				break;
			case 1:
				if (c == ';') {
					name = chunk;
					chunk = "";
					section = 2;
					break;
				}
				chunk += c;
				break;
			case 2:
				if (c == ';') {
					id = std::stoi(chunk);
					chunk = "";
					section = 3;
					break;
				}
				chunk += c;
				break;
			case 3:
				if (c == ';') {
					armies = std::stoi(chunk);
					chunk = "";
					break;
				}
				chunk += c;
				break;
			default:
				break;
			}
		}
		for (int i = 0; i < xyPairs.size(); i++) {
			std::cout << "X,Y: " << xyPairs.at(i).at(0) << "," << xyPairs.at(i).at(1) << std::endl;
		}
		xyPairs.clear();
		std::cout << "Name: " << name << std::endl << "ID: " << id << std::endl << "Army Size: " << armies << std::endl << std::endl;
	}
}

Territory World::GetTerritory(int index)
{
    return territoryList.at(index);
}

unsigned int World::TerritoryNumber()
{
     return territoryList.size();
}

unsigned int World::PlayerNumber()
{
    return playerList.size();
}

Player World::GetPlayer(int index)
{
    return playerList.at(index);
}
// World

////////////////////////////////////////////////////////////////////////////////////////////
