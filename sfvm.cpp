#include "sfvm.hpp"

namespace sfvm
{

    sf::Vector2f i(1.0f, 0.0f);
    sf::Vector2f j(0.0f, 1.0f);

    // dot product -- should override this for all types
    float Dot(sf::Vector2f v1, sf::Vector2f v2)
    {
        return 0.0f;
    }

    // cross product -- should override this for all types
    sf::Vector2f Cross(sf::Vector2f v1, sf::Vector2f v2)
    {
        return sf::Vector2f(0, 0);
    }

}
