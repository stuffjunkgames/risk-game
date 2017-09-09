#include "risk.hpp"
#include "sfvm.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>

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

void Player::AddTerritory(Territory* captured, unsigned int army)
{
    captured->SetOwner(this, army);
    territories.push_back(captured->getID());
}

void Player::CaptureTerritory(Territory *captured, unsigned int army)
{
    captured->ChangeOwner(this, army);
    territories.push_back(captured->getID());
}

void Player::LostTerritory(Territory *captured)
{
    territories.remove(captured->getID());
}

unsigned int Player::getNumTerritories()
{
    return this->territories.size();
}

sf::Color Player::getColor()
{
    return this->color;
}

int Player::getID()
{
    return this->playerNumber;
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

    this->RefreshText();
}

// set text number and position
void Territory::RefreshText()
{
    armyDisplay.setString(std::to_string(army));

    // set text position to center of shape
    sf::FloatRect bounds = armyDisplay.getGlobalBounds();
    sf::Vector2f textShift(bounds.width / 2, bounds.height / 2);
    //sf::Vector2f centroid = this->Centroid();

    //armyDisplay.setPosition(centroid - textShift);
	armyDisplay.setPosition(centerPos - textShift);
}

// if the territory object is copied, this needs to be called again
void Territory::setFont()
{
    // set text position to center of shape
    sf::FloatRect bounds = armyDisplay.getGlobalBounds();
    sf::Vector2f textShift(bounds.width / 2, bounds.height / 2);
    //sf::Vector2f centroid = this->Centroid();
	
    //armyDisplay.setPosition(centroid - textShift);
	armyDisplay.setPosition(centerPos - textShift);
}

// override Drawable::draw() to make this more consistent with SFML standard
// setFont MUST be called BEFORE calling draw, or drawing text will not work (probably segfault)
void Territory::drawTerritory(sf::RenderWindow* window)
{
	this->territorySprite.setColor(this->owner->getColor());
	window->draw(this->territorySprite);
    window->draw(armyDisplay);
}

void Territory::addConnection(Territory *connection)
{
    connected.push_back(connection);
}

void Territory::ChangeOwner(Player *newOwner, unsigned int newArmy)
{
    owner->LostTerritory(this);
    owner = newOwner;
    army = newArmy;
    this->setFillColor(owner->color);
	this->RefreshText();
}

void Territory::SetOwner(Player *newOwner, unsigned int newArmy)
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

int Territory::getID()
{
    return id;
}

bool Territory::isConnected(Territory *t)
{
    std::vector<Territory*>::iterator iter;
    for(iter = connected.begin(); iter < connected.end(); iter++)
    {
        if(*iter == t)
        {
            return true;
        }
    }

    return false;

}

unsigned int Territory::GetArmies()
{
    return army;
}

std::string Territory::getName()
{
	return this->name;
}

void Territory::setCenter(sf::Vector2f pos)
{
	this->centerPos = pos;
}

void Territory::makeArrows()
{
	for(unsigned int i = 0; i < connected.size(); i++)
	{
		attackArrows.push_back(Arrow(centerPos, connected.at(i)->centerPos));
	}
}

void Territory::drawArrows(sf::RenderWindow* window)
{
	for (unsigned int i = 0; i < attackArrows.size(); i++)
	{
		window->draw(attackArrows.at(i));
	}
}

void Territory::SetBonus(int b)
{
    this->bonus = b;
}

int Territory::GetBonus()
{
    return this->bonus;
}

// Territory class ^
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Bonus class

Bonus::Bonus(){}
Bonus::Bonus(int id, int bonus, std::string name)
{
    this->id = id;
    this->bonus = bonus;
    this->name = name;
}

void Bonus::AddTerritory(int id)
{
    this->territories.push_back(id);
}

void Bonus::SetBonus(int n)
{
    this->bonus = n;
}

void Bonus::SetName(std::string name)
{
    this->name = name;
}

std::string Bonus::GetName()
{
    return this->name;
}

void Bonus::SetID(int id)
{
    this->id = id;
}

int Bonus::GetID()
{
    return this->id;
}

int Bonus::GetBonus()
{
    return this->bonus;
}

std::vector<int> Bonus::GetTerritories()
{
    return this->territories;
}

// Bonus class ^
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// World class

World::World(sf::Font& font)
{
    // make the world.
    // TODO: read in from a file (JSON?) or database (SQL?)

    Player player1(0, "Player 1", sf::Color::Blue);
    Player player2(1, "Player 2", sf::Color::Red);
	Player player3(2, "Player 3", sf::Color::Green);
	Player player4(3, "Player 4", sf::Color(255,165,0,255));

    playerList.push_back(player1);
    playerList.push_back(player2);
	playerList.push_back(player3);
	playerList.push_back(player4);

	ReadFile(font);

    playerTurn = -1;
}

void World::ReadFile(sf::Font& font)
{
    // FIXME: this would be way easier to implement with streams than trying to use characters.
    // FIXME: also might be nicer to have a more standard format like JSON

	std::ifstream infile("world.txt");
	std::string line;
	std::string chunk;

	// read territories
	int n = 0;
	while(std::getline(infile, line))
	{
		std::istringstream iss(line);
		//std::cout << line << std::endl;

		//parsing
		chunk = "";
		int section = 0;// 0:line identifier (t=territory, c=connection, b=bonus)
                         // 1:coordinates, 2:name, 3:id, 4:armysize, 5: bonus
                         // -1: territory, -2: connected
                         // 11: bonus id, 12: name, 13: bonus income
        char id;

		int x, y; //coordinates
		std::vector<std::vector<int> > xyPairs;
		std::string name;
		int player;
		int armies;
		int bonus;

		int bonusID;
		std::string bonusName;
		int bonusIncome;

		int terr;
		std::vector<int> connections;

		for (char c : line)
		{
			switch(section)
			{
            case 0:
                if(c == 't')
                {
                    id = 't';
                }
                else if(c== 'c')
                {
                    id = 'c';
                }
                else if(c == 'b')
                {
                    id = 'b';
                }
                else if(c == ';')
                {
                    if(id == 't')
                    {
                        section = 1;
                    }
                    else if(id == 'c')
                    {
                        section = -1;
                    }
                    else if(id == 'b')
                    {
                        section = 11;
                    }
                }
                break;
			case 1:
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
						section = 2;
					break;
				}
				chunk += c;
				break;
			case 2:
				if (c == ';') {
					name = chunk;
					chunk = "";
					section = 3;
					break;
				}
				chunk += c;
				break;
			case 3:
				if (c == ';') {
					player = std::stoi(chunk);
					chunk = "";
					section = 4;
					break;
				}
				chunk += c;
				break;
			case 4:
				if (c == ';') {
					armies = std::stoi(chunk);
					chunk = "";
					section = 5;
					break;
				}
				chunk += c;
				break;
            case 5:
                // bonus
                if(c == ';')
                {
                    bonus = std::stoi(chunk);
                    chunk = "";
                    break;
                }
                chunk += c;
                break;
            case 11:
                // bonus id
                if(c == ';')
                {
                    bonusID = std::stoi(chunk);
                    chunk = "";
                    section = 12;
                    break;
                }
                chunk += c;
                break;
            case 12:
                // bonus name
                if(c == ';')
                {
                    bonusName = chunk;
                    chunk = "";
                    section = 13;
                    break;
                }
                chunk += c;
                break;
            case 13:
                // bonus income
                if(c == ';')
                {
                    bonusIncome = std::stoi(chunk);
                    chunk = "";
                    break;
                }
                chunk += c;
                break;
            case -1:
                if(c == ';')
                {
                    terr = std::stoi(chunk);
                    section--;
                    chunk = "";
                    break;
                }
                chunk += c;
                break;
            case -2:
                if(c == ' ')
                {
                    connections.push_back(std::stoi(chunk));
                    chunk = "";
                    break;
                }
                else if(c == ';')
                {
                    connections.push_back(std::stoi(chunk));
                    section--;
                    break;
                }

                chunk += c;
                break;
			default:
				break;
			}
		}
		if(id == 't')
        {
            // add territory
            territoryList.push_back(Territory(xyPairs.size(), n, name, &playerList[player], armies, font));

            for (unsigned int i = 0; i < xyPairs.size(); i++) {
                std::cout << "X,Y: " << xyPairs.at(i).at(0) << "," << xyPairs.at(i).at(1) << std::endl;
                //territoryList.back().setPoint(i, sf::Vector2f(xyPairs.at(i).at(0), xyPairs.at(i).at(1)));
				territoryList.back().setCenter(sf::Vector2f(xyPairs.at(i).at(0), xyPairs.at(i).at(1)));
            }
            territoryList.back().RefreshText();
            playerList[player].AddTerritory(&territoryList.back(), armies);
            bonusList[bonus].AddTerritory(n);
            territoryList.back().SetBonus(bonus);
            xyPairs.clear();
            std::cout << "Name: " << name << std::endl << "ID: " << n << std::endl << "Army Size: " << armies << std::endl;
            std::cout << "Bonus: " << bonusList[bonus].GetName() << std::endl << std::endl;
            n++;
        }
        else if(id == 'c')
        {
            std::cout << terr << " ";
            // add connection
            while(connections.size() > 0)
            {
                (territoryList.at(terr)).addConnection(&territoryList.at(connections.back()));
                std::cout << connections.back() << " ";
                connections.pop_back();
            }
            std::cout << std::endl;
			territoryList.at(terr).makeArrows();
        }
        else if(id == 'b')
        {
            // add bonus
            bonusList.push_back(Bonus(bonusID, bonusIncome, bonusName));

            std::cout << "Bonus: " << bonusName << std::endl << "ID: " << bonusID << std::endl << "Income: " << bonusIncome << std::endl << std::endl;
        }

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

Player* World::getPlayerID(int id)
{
    for(unsigned int i = 0; i < this->playerList.size(); i++)
    {
        if(this->playerList[i].getID() == id)
        {
            return &(this->playerList[i]);
        }
    }

    // FIXME: return null player here
    return &(playerList[0]);
}

// This assumes player numbers are 0-indexed and do not skip numbers
Player* World::getNextPlayer()
{
    if(playerTurn < 0)
    {
        // initialize to first player
        playerTurn = 0;
    }

    int tempTurn = playerTurn;

    //FIXME: account for null player (playerList.size() will be too high)
    playerTurn = (playerTurn + 1) % playerList.size();

    return getPlayerID(tempTurn);
}

int World::getSize()
{
	return territoryList.size();
}

int World::GetBonus(int pNumber)
{
    int bonus = 0;
    for(unsigned int i = 0; i < bonusList.size(); i++)
    {
        std::vector<int> bonusTList = bonusList[i].GetTerritories();
        bool owned = 1;
        for(unsigned int j = 0; j < bonusTList.size(); j++)
        {
            if(pNumber != getTerritory(bonusTList[j])->GetOwner()->getID())
            {
                owned = 0;
                break;
            }
        }
        if(owned)
        {
            bonus += bonusList[i].GetBonus();
        }
    }

    return bonus;
}

std::string World::GetBonusName(Territory* t)
{
    int id = t->GetBonus();
    std::string name = "";

    for(unsigned int i = 0; i < bonusList.size(); i++)
    {
        if(id == bonusList[i].GetID())
        {
            name = bonusList[i].GetName();
            break;
        }
    }

    return name;
}

std::string World::GetBonusName(int tid)
{
    // FIXME: use territory id instead of index
    return GetBonusName(getTerritory(tid));
}

int World::GetBonusIncome(Territory* t)
{
    int id = t->GetBonus();
    int income = 0;

    for(unsigned int i = 0; i < bonusList.size(); i++)
    {
        if(id == bonusList[i].GetID())
        {
            income = bonusList[i].GetBonus();
            break;
        }
    }

    return income;
}

int World::GetBonusIncome(int tid)
{
    // FIXME: use territory id instead of index
    return GetBonusIncome(getTerritory(tid));
}

// World
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Arrow class definitions

Arrow::Arrow() : ExtendedShape()
{

}

Arrow::Arrow(sf::Vector2f center1, sf::Vector2f center2) : ExtendedShape(3)
{
	int dx = (center2.x - center1.x);
	int dy = (center2.y - center1.y);
	sf::Vector2f newCenter1 = center1 + sf::Vector2f((2.0 / 5.0) * dx, (2.0 / 5.0) * dy);
	sf::Vector2f newCenter2 = center2 - sf::Vector2f((1.0 / 5.0) * dx, (1.0 / 5.0) * dy);
	dx = (newCenter2.x - newCenter1.x);
	dy = (newCenter2.y - newCenter1.y);
	// length
	float l = sqrt(dx * dx + dy * dy);
	// angle, theta
	float t = atan2(dy, dx);
	float pi = 3.1415926535897;
	float scale = 2 * sqrt(l);

	this->setPoint(0, newCenter1 + sf::Vector2f(scale * cos(-(pi / 2 - t)), scale * sin(-(pi / 2 - t))));
	this->setPoint(1, newCenter2);
	this->setPoint(2, newCenter1 - sf::Vector2f(scale * cos(-(pi / 2 - t)), scale * sin(-(pi / 2 - t))));

	this->setFillColor(sf::Color(255, 0, 0, 127));
	setOutlineThickness(-4);
	setOutlineColor(sf::Color(255, 255, 0, 127));
}

void Arrow::Draw(sf::RenderWindow* window)
{
	window->draw(*this);
}

void Arrow::setActive()
{
	setFillColor(sf::Color(255, 0, 0, 255));
	setOutlineColor(sf::Color(255, 255, 0, 255));
}

// Arrow
////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// HoverText class definitions

HoverText::HoverText(sf::Font& font)
{
	rect = ExtendedShape(4);
	rect.setFillColor(sf::Color(0, 0, 0, 127));

	this->setFont(font);
	this->setCharacterSize(20);
	this->setFillColor(sf::Color::White);

	offset = 10;
}

void HoverText::Draw(sf::RenderWindow* window)
{
	sf::FloatRect bounds = getGlobalBounds();

	rect.setPoint(0, sf::Vector2f(bounds.left - offset, bounds.top - offset));
	rect.setPoint(1, sf::Vector2f(bounds.left - offset, bounds.top + bounds.height + offset));
	rect.setPoint(2, sf::Vector2f(bounds.left + bounds.width + offset, bounds.top + bounds.height + offset));
	rect.setPoint(3, sf::Vector2f(bounds.left + bounds.width + offset, bounds.top - offset));

	window->draw(rect);
	window->draw(*this);
}

void HoverText::setText(std::string text, int xPos, int yPos)
{
	this->setString(text);

	sf::FloatRect bounds = getGlobalBounds();

	this->setPosition(sf::Vector2f(xPos - (bounds.width / 2), yPos - bounds.height - 2 * offset));
}
// HoverText
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Label class definitions

Label::Label()
{
	this->setCharacterSize(20);
	this->setFillColor(sf::Color::White);
	this->setOutlineColor(sf::Color::Black);
	this->setOutlineThickness(3);
}

Label::Label(sf::Font& font)
{
	this->setFont(font);
	this->setCharacterSize(20);
	this->setFillColor(sf::Color::White);
	this->setOutlineColor(sf::Color::Black);
	this->setOutlineThickness(3);
}
// Label
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Transfer class definitions

Transfer::Transfer(sf::Font& font, int donor, int receiver, Arrow arrow)
{
	transferLabel.setFont(font);
	this->amount = 0;
	this->donor = donor;
	this->receiver = receiver;
	this->transferArrow = arrow;

	transferArrow.setActive();
	transferLabel.setString(std::to_string(amount));

	sf::FloatRect bounds = transferLabel.getGlobalBounds();
	sf::Vector2f textShift(bounds.width / 2, bounds.height / 2);
	transferLabel.setPosition(transferArrow.Centroid() - textShift);
}

void Transfer::Draw(sf::RenderWindow* window)
{
	window->draw(transferArrow);
	transferLabel.setString(std::to_string(amount));
	
	window->draw(transferLabel);	
}

void Transfer::setAmount(int amount)
{
	this->amount = amount;
}

void Transfer::increaseAmount(int inc)
{
	this->amount += inc;
}

int Transfer::getAmount()
{
	return this->amount;
}

bool Transfer::operator == (const Transfer &other)
{
	return (this->donor == other.donor && this->receiver == other.receiver);
}
// Transfer
////////////////////////////////////////////////////////////////////////////////////////////