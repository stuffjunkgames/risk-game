#ifndef MOVEMENT_HPP_INCLUDED
#define MOVEMENT_HPP_INCLUDED

#include "risk.hpp"

enum TurnPhase {
    place,
    attack,
    reposition
};

int Place(World* world, World* initialWorld, Player* player, int territory, int armies, int remaining);
int Attack(World* world, Player* player, int source, int target, int armies);
int Reposition(World* world, World* initialWorld, Player* player, int source, int target, int armies);

#endif // MOVEMENT_HPP_INCLUDED
