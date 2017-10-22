#ifndef RISK_HPP_INCLUDED
#define RISK_HPP_INCLUDED


#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <list>
#include <string>

class Player;
class Territory;
class World;
class ExtendedShape;
class Arrow;
class HoverText;
class Label;
class Transfer;
class Button;

class ExtendedShape : public sf::ConvexShape
{
public:
    ExtendedShape();
    ExtendedShape(int n);

    float Area();
    sf::Vector2f Centroid();
    bool isInside(sf::Vector2f point);
};

class HoverText : public sf::Text
{
	ExtendedShape rect;
	int offset;

public:
	// constructor
	HoverText(sf::Font& font);

	// public functions
	void Draw(sf::RenderWindow* window);
	void setText(std::string text, int xPos, int yPos);
};

class Label : public sf::Text
{

public:
	// constructor
	Label();
	Label(sf::Font& font);

	// public functions
};

class Player
{
    int playerNumber;
    std::string name;
    std::list<int> territories; // territory ids owned by this player

public:
    sf::Color color;    // make this private

	Player();
    Player(int number, std::string name, sf::Color color);

    void AddTerritory(Territory* captured, unsigned int army);
    void CaptureTerritory(Territory *captured, unsigned int army);
    void LostTerritory(Territory *captured);
    bool operator == (const Player &other);
    sf::Color getColor();
    unsigned int getNumTerritories();
    int getID();
	std::string getName();

}; // Player

class Territory : public ExtendedShape  // if we make a ConcaveShape, inherit from that instead
{
    // private variables
    int id;
    std::string name;

    Player* owner;
    unsigned int army;  // how many armies player has in territory
    int bonus;
    std::vector<Territory*> connected;
	std::vector<Arrow> attackArrows;

public:
    // constructor
    Territory();
    Territory(int n);
    Territory(int n, int id, std::string name, Player* player, unsigned int army, sf::Font& font);

    // public functions
    void Initialize(int id, std::string name, Player* player, unsigned int army, sf::Font& font);

    void RefreshText();
    void addConnection(Territory *connection);
    void SetOwner(Player *newOwner, unsigned int newArmy);
    void ChangeOwner(Player *newOwner, unsigned int newArmy);
    void drawTerritory(sf::RenderWindow* window);
    void setFont();     // should be able to remove this
	void AddArmies(int nArmies);
	void makeArrows();
	void drawArrows(sf::RenderWindow* window);
	void SetBonus(int b);
	std::string getName();
	int getID();
	int GetBonus();
	Player* GetOwner();
	bool isConnected(Territory* t);
	unsigned int GetArmies();
	void setCenter(sf::Vector2f pos);
	std::vector<Territory*>* getConnected();

    bool operator == (const Territory& other);

	Label armyDisplay;

	sf::Vector2f centerPos;
	sf::Texture territoryTexture;
	sf::Texture borderTexture;

	sf::Sprite territorySprite;
	sf::Sprite borderSprite;

	sf::Image territoryImage;
}; // Territory

class Bonus
{
    int id;
    std::string name;
    std::vector<int> territories;   // list of territory ids in this bonus
    int bonus;  // bonus armies per turn for owning

public:
    Bonus();
    Bonus(int id, int bonus, std::string name);

    void AddTerritory(int id);
    void SetBonus(int n);
    void SetName(std::string name);
    std::string GetName();
    void SetID(int id);
    int GetID();
    std::vector<int> GetTerritories();
    int GetBonus();
};  // Bonus

class World
{
    std::vector<Territory> territoryList;
    std::vector<Player> playerList;
    std::vector<Bonus> bonusList;
    int playerTurn;
	Player nullPlayer;

public:
	World();
	World(sf::Font& font);
    World(sf::Font& font, std::vector<std::string> playerNames, std::vector<sf::Color> playerColors);

    Territory* getTerritory(int index);
    unsigned int TerritoryNumber();
    unsigned int PlayerNumber();
    Player* getPlayer(int index);
    Player* getPlayerID(int id);
    Player* getNextPlayer();
	void ReadFile(sf::Font& font);
	int getSize();
	int GetBonus(int pNumber);
	std::string GetBonusName(Territory* t);
	std::string GetBonusName(int tid);
	int GetBonusIncome(Territory* t);
	int GetBonusIncome(int tid);
	int addPlayer(std::string playerName, sf::Color playerColor);
	int addPlayer(int id, std::string playerName, sf::Color playerColor);
	void allocateTerritories();
}; // World


class Arrow : public ExtendedShape
{

public:
	// constructor
	Arrow();
	Arrow(sf::Vector2f center1, sf::Vector2f center2);

	// public functions
	void Draw(sf::RenderWindow* window);
	void setActive();
};

class DashedLine
{
	sf::Color fillColor = sf::Color(255, 255, 0, 255);
	std::vector<ExtendedShape> dashes;
	sf::Vector2f startPos, endPos;
	int maxDashes = 30;
	int dashWidth = 10;
	int dashLength = 50;
	float spacing = 1.5f;

public:
	DashedLine();
	DashedLine(sf::Vector2f startPos, sf::Vector2f endPos);
	void Draw(sf::RenderWindow* window, sf::Vector2f startPos, sf::Vector2f endPos);
	void Draw(sf::RenderWindow* window);
	sf::Vector2f getCenter();
	void setPoints(sf::Vector2f startPos, sf::Vector2f endPos);
};

class Transfer
{
	DashedLine transferLine;
	Label transferLabel;
	sf::Vector2f position;
	int amount;
public:
	// constructor
	Transfer(sf::Font& font, int donor, int receiver, DashedLine line);

	// public functions
	void Draw(sf::RenderWindow* window);
	void setAmount(int amount);
	void increaseAmount(int inc);
	int getAmount();
	void setLinePoints(sf::Vector2f startPos, sf::Vector2f endPos);
	bool operator == (const Transfer &other);

	int donor;
	int receiver;
};

class Button : public ExtendedShape
{
	Label text;
	int defaultWidth = 30;
	int defaultHeight = 30;
	sf::Color fillColor = sf::Color(255, 255, 255, 127);

public:
	// constructor
	Button(sf::Font& font, std::string string, sf::Vector2f position);
	Button(sf::Font& font, std::string string, sf::Vector2f position, int w, int h);

	// public functions
	void Draw(sf::RenderWindow* window);
	void moveToPosition(sf::Vector2f newPosition);
	void setString(std::string str);
	void appendString(std::string str);
	void clearText();
	void setCharacterSize(int charSize);
	Label* getLabel();

	bool isActive = false;
};

class TextEntry : public Button
{
	int maxChars;

public:
	TextEntry(sf::Font& font, sf::Vector2f position, int w, int h, int maxChars);
	void appendString(std::string str);
	void subtractString();
	void setIsTyping(bool isTyping);
};

class ColorPalette : public ExtendedShape
{
	std::vector<ExtendedShape> shapes;
	int size = 100;
	int colorSelected;

	std::vector<sf::Color> colors{ sf::Color(59, 160, 176, 255),
		sf::Color(176, 59, 110, 255),
		sf::Color(114, 181, 60, 255),
		sf::Color(176, 90, 59, 255) };

public:
	ColorPalette(sf::Vector2f position, int nColors);
	void Draw(sf::RenderWindow* window);
	bool isInside(sf::Vector2f point);
	sf::Color getSelectedColor();
	void chooseColor(int n);
};

#endif // RISK_HPP_INCLUDED
