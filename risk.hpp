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

    Player(int number, std::string name, sf::Color color);

    void Initialize(int number, std::string name, sf::Color color);
    void AddTerritory(Territory* captured, unsigned int army);
    void CaptureTerritory(Territory *captured, unsigned int army);
    void LostTerritory(Territory *captured);
    bool operator == (const Player &other);
    sf::Color getColor();
    unsigned int getNumTerritories();
    int getID();

}; // Player

class Territory : public ExtendedShape  // if we make a ConcaveShape, inherit from that instead
{
    // private variables
    int id;
    std::string name;
    Label armyDisplay;
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
    //virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
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

    bool operator == (const Territory& other);
	sf::Vector2f centerPos;
	sf::Texture territoryTexture;
	sf::Texture borderTexture;

	sf::Sprite territorySprite;
	sf::Sprite borderSprite;

	sf::Image territoryImage;

    // add function for adding/subtracting armies

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

public:
    World(sf::Font& font);

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

class Transfer
{
	Arrow transferArrow;
	Label transferLabel;
	sf::Vector2f position;
	int amount;
	Territory* donor,* receiver;
	Player* owner;
public:
	// constructor
	Transfer(sf::Font font, int amount, Territory* donor, Territory* receiver, Player* owner);

	// public functions
	void Draw(sf::RenderWindow* window);
	void setAmount(int amount);
	void increaseAmount(int inc);
	int getAmount();
};