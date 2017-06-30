#include "sfvm.hpp"

namespace sfvm
{

    sf::Vector3f i(1.0f, 0.0f, 0.0f);
    sf::Vector3f j(0.0f, 1.0f, 0.0f);
    sf::Vector3f k(0.0f, 0.0f, 1.0f);

    // dot product -- should override this for all types
    float Dot(sf::Vector2f v1, sf::Vector2f v2)
    {
        return 0.0f;
    }

    // cross product -- should override this for all types
    // 3d cross product of 2d vectors in xy plane
    sf::Vector3f Cross(sf::Vector2f u, sf::Vector2f v)
    {
        u1 = u.x;
        u2 = u.y;
        u3 = 0;
        v1 = v.x;
        v2 = v.y;
        v3 = 0;

        sf::Vector3f w;

        w.x = u2*v3 - u3*v2;
        w.y = u3*v1 - u1*v3;
        w.z = u1*v2 - u2*v1;

        return w;
    }

}
