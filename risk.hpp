#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <list>

class Player;
class Territory;
class ExtendedShape;

class ExtendedShape : public sf::ConvexShape
{
public:
    ExtendedShape();
    ExtendedShape(int n);

    float Area();
    sf::Vector2f Centroid();
    bool isInside(sf::Vector2f point);


};

class Player
{
    int playerNumber;
    std::string name;
    std::list<Territory> territories; // territories owned by this player

public:
    sf::Color color;    // make this private

    Player(int number, std::string name, sf::Color color);

    void Initialize(int number, std::string name, sf::Color color);
    void CaptureTerritory(Territory *captured, unsigned int army);
    void LostTerritory(Territory *captured);
    bool operator == (const Player &other);
    sf::Color getColor();

}; // Player

class Territory : public ExtendedShape  // if we make a ConcaveShape, inherit from that instead
{
    // private variables
    int id;
    sf::Text armyDisplay;
    std::string name;
    Player* owner;
    unsigned int army;  // how many armies player has in territory
    std::vector<Territory> connected;

public:
    // constructor
    Territory();
    Territory(int n);
    Territory(int n, int id, std::string name, Player* player, unsigned int army, sf::Font& font);

    // public functions
    void Initialize(int id, std::string name, Player* player, unsigned int army, sf::Font& font);

    void RefreshText();
    void addConnection(Territory connection);
    void ChangeOwner(Player *newOwner, unsigned int newArmy);
    //virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void drawTerritory(sf::RenderWindow* window);
    void setFont();     // should be able to remove this

    bool operator == (const Territory& other);

    // add function for adding/subtracting armies

}; // Territory

class World
{
    std::vector<Territory> territoryList;
    std::vector<Player> playerList;

public:
    World(sf::Font& font);

    Territory* getTerritory(int index);
    unsigned int TerritoryNumber();
    unsigned int PlayerNumber();
    Player* getPlayer(int index);
	void ReadFile(sf::Font& font);
}; // World
