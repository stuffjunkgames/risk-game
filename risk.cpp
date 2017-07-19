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
// Territory class definitions

bool Territory::operator == (const Territory &other)
{
    return this->id == other.id;
}

Territory::Territory() : ExtendedShape()
{
    // call ExtendedShape constructor
}

Territory::Territory(int n) : ExtendedShape(n)
{
    // call ExtendedShape constructor
}

Territory::Territory(int n, int id, std::string name, Player* player, unsigned int army, sf::Font& font) : ExtendedShape(n)
{
    // call ExtendedShape constructor

    this->id = id;
    this->name = name;
    this->owner = player;
    this->army = army;
    this->setFillColor(owner->color);
	setOutlineThickness(-2);
	setOutlineColor(sf::Color::Black);

    // set font
    armyDisplay.setFont(font);

    // set display text
    armyDisplay.setCharacterSize(20);
    armyDisplay.setString(std::to_string(army));
}

void Territory::Initialize(int id, std::string name, Player* player, unsigned int army, sf::Font& font)
{
    this->id = id;
    this->name = name;
    this->owner = player;
    this->army = army;
    this->setFillColor(owner->color);

    // make sure Text's reference to Font is correct
    armyDisplay.setFont(font);

    // set display text
    armyDisplay.setCharacterSize(12);
    armyDisplay.setString(std::to_string(army));

    // set text position to center of shape
    sf::FloatRect bounds = armyDisplay.getGlobalBounds();
    sf::Vector2f textShift(bounds.width / 2, bounds.height / 2);
    sf::Vector2f centroid = this->Centroid();

    armyDisplay.setPosition(centroid - textShift);
    armyDisplay.setFillColor(sf::Color::White);
}

// set text number and position
void Territory::RefreshText()
{
    armyDisplay.setString(std::to_string(army));

    // set text position to center of shape
    sf::FloatRect bounds = armyDisplay.getGlobalBounds();
    sf::Vector2f textShift(bounds.width / 2, bounds.height / 2);
    sf::Vector2f centroid = this->Centroid();

    armyDisplay.setPosition(centroid - textShift);
    armyDisplay.setFillColor(sf::Color::White);
}

// if the territory object is copied, this needs to be called again
void Territory::setFont()
{
    // set text position to center of shape
    sf::FloatRect bounds = armyDisplay.getGlobalBounds();
    sf::Vector2f textShift(bounds.width / 2, bounds.height / 2);
    sf::Vector2f centroid = this->Centroid();

    armyDisplay.setPosition(centroid - textShift);
    armyDisplay.setFillColor(sf::Color::White);
}

// override Drawable::draw() to make this more consistent with SFML standard
// setFont MUST be called BEFORE calling draw, or drawing text will not work (probably segfault)
void Territory::drawTerritory(sf::RenderWindow* window)
{
    window->draw(*this);
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
    this->setFillColor(owner->color);
	this->RefreshText();
}

void Territory::AddArmies(int nArmies)
{
	army += nArmies;
	this->RefreshText();
}

Player* Territory::GetOwner()
{
	return owner;
}

// Territory class ^
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// World class

World::World(sf::Font& font)
{
    // make the world.
    // TODO: read in from a file (JSON?) or database (SQL?)

    Player player1(1, "Player 1", sf::Color::Blue);
    Player player2(2, "Player 2", sf::Color::Red);

    playerList.push_back(player1);
    playerList.push_back(player2);

	ReadFile(font);
	/*
    Territory t1(4, 1, "Territory 1", &playerList[0], 12, font);
    Territory t2(4, 2, "Territory 2", &playerList[1], 5, font);
    Territory t3(4, 3, "Territory 3", &playerList[1], 10, font);
    Territory t4(4, 4, "Territory 4", &playerList[0], 20, font);

    t1.setPoint(0, sf::Vector2f(0,0));
    t1.setPoint(1, sf::Vector2f(0,100));
    t1.setPoint(2, sf::Vector2f(100,100));
    t1.setPoint(3, sf::Vector2f(100,0));
    t1.RefreshText();

    territoryList.push_back(t1);

    t2.setPoint(0, sf::Vector2f(100,0));
    t2.setPoint(1, sf::Vector2f(100,100));
    t2.setPoint(2, sf::Vector2f(200,100));
    t2.setPoint(3, sf::Vector2f(200,0));
    t2.RefreshText();

    territoryList.push_back(t2);

    t3.setPoint(0, sf::Vector2f(0,100));
    t3.setPoint(1, sf::Vector2f(0,200));
    t3.setPoint(2, sf::Vector2f(100,200));
    t3.setPoint(3, sf::Vector2f(100,100));
    t3.RefreshText();

    territoryList.push_back(t3);

    t4.setPoint(0, sf::Vector2f(100,100));
    t4.setPoint(1, sf::Vector2f(100,200));
    t4.setPoint(2, sf::Vector2f(200,200));
    t4.setPoint(3, sf::Vector2f(200,100));
    t4.RefreshText();

    territoryList.push_back(t4);
    */

}

void World::ReadFile(sf::Font& font)
{
	std::ifstream infile("world.txt");
	std::string line;
	std::string chunk;
	for (int n = 0; std::getline(infile, line); n++)
	{
		std::istringstream iss(line);
		//std::cout << line << std::endl;

		//parsing
		chunk = "";
		char section = 0;// 0:coordinates, 1:name, 2:id, 3:armysize
		int x, y; //coordinates
		std::vector<std::vector<int> > xyPairs;
		std::string name;
		int player;
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
					player = std::stoi(chunk);
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
		territoryList.push_back(Territory(xyPairs.size(), n, name, &playerList[player], armies, font));
		for (unsigned int i = 0; i < xyPairs.size(); i++) {
			std::cout << "X,Y: " << xyPairs.at(i).at(0) << "," << xyPairs.at(i).at(1) << std::endl;
			territoryList.back().setPoint(i, sf::Vector2f(xyPairs.at(i).at(0), xyPairs.at(i).at(1)));
		}
		territoryList.back().RefreshText();
		xyPairs.clear();
		std::cout << "Name: " << name << std::endl << "ID: " << n << std::endl << "Army Size: " << armies << std::endl << std::endl;

	}
}

Territory* World::getTerritory(int index)
{
    return &(territoryList.at(index));
}

unsigned int World::TerritoryNumber()
{
     return territoryList.size();
}

unsigned int World::PlayerNumber()
{
    return playerList.size();
}

Player* World::getPlayer(int index)
{
    return &(playerList.at(index));
}
// World

////////////////////////////////////////////////////////////////////////////////////////////
