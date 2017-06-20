#include <SFML/System/Vector2.hpp>

namespace sfvm
{

    // dot product -- should override this for all types
    float Dot(sf::Vector2f v1, sf::Vector2f v2);

    // cross product -- should override this for all types
    sf::Vector2f Cross(sf::Vector2f v1, sf::Vector2f v2);

}
