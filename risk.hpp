#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>

class Player;
class Territory;

class ExtendedShape : public sf::ConvexShape
{
public:
    ExtendedShape();
    ExtendedShape(int n);

    float Area();
    sf::Vector2f Centroid();
};

class Territory
{
    // private variables
    int id;
    ExtendedShape territory;
    sf::Text armyDisplay;
    sf::Font armyFont;
    std::string name;
    Player *owner;
    unsigned int army;  // how many armies player has in territory
    std::vector<Territory> connected;

public:
    // constructor
    Territory(int id, ExtendedShape *shape, std::string name, Player *player, unsigned int army);

    // public functions
    void addConnection(Territory connection);
    void ChangeOwner(Player *newOwner, unsigned int newArmy);
    void draw(sf::RenderWindow *window);
    ExtendedShape getShape();
    void setFont();
    bool operator == (const Territory &other);

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
    sf::Color color;    // make this private

    Player(int number, std::string name, sf::Color color);
    void CaptureTerritory(Territory *captured, unsigned int army);
    void LostTerritory(Territory *captured);
    bool operator == (const Player &other);
    sf::Color getColor();

}; // Player

class World
{
    std::vector<Territory> territoryList;
    std::vector<Player> playerList;

public:
    World();

    Territory getTerritory(int index);
    unsigned int territoryNumber();
}; // World
