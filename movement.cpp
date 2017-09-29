#include "movement.hpp"

// return number of armies actually placed
int Place(World* world, World* initialWorld, Player* player, int territory, int armies, int remaining)
{
    if(world->getTerritory(territory)->GetOwner() != player)
    {
        // player doesn't own the territory
        return 0;
    }

    int alreadyPlaced = world->getTerritory(territory)->GetArmies() - initialWorld->getTerritory(territory)->GetArmies();
    if(armies < -alreadyPlaced)
    {
        // can only remove what is already placed
        world->getTerritory(territory)->AddArmies(-alreadyPlaced);
        return -alreadyPlaced;
    }

    if(armies > remaining)
    {
        // can only place what you have
        world->getTerritory(territory)->AddArmies(remaining);
        return remaining;
    }

    world->getTerritory(territory)->AddArmies(armies);
    return armies;
}

// return actual number attacked with
int Attack(World* world, Player* player, int source, int target, int armies)
{
    if(world->getTerritory(source)->GetOwner() != player || world->getTerritory(target)->GetOwner() == player)
    {
        // can't attack self, or with from an unowned territory
        return 0;
    }

    int attacking = armies;
    if(attacking > (int)world->getTerritory(source)->GetArmies())
    {
        attacking = world->getTerritory(source)->GetArmies();
    }

    int i = 0;
    while(i < attacking)
    {
        int r = rand();
        if(r > RAND_MAX / 2)
        {
            // kill target
            world->getTerritory(target)->AddArmies(-1);
            if(world->getTerritory(target)->GetArmies() <= 0)
            {
                player->CaptureTerritory(world->getTerritory(target), 1);
                world->getTerritory(source)->AddArmies(-1);
                break;
            }
        }
        else
        {
            // kill source
            world->getTerritory(source)->AddArmies(-1);
        }

        i++;
    }

    return attacking;
}

// TODO implement this.
int Reposition(World* world, World* initialWorld, Player* player, int source, int target, int armies)
{
    return 0;
}
