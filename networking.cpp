#include "networking.hpp"

// request move
sf::Packet ClientRequestMove(int source, int target, int army)
{
    sf::Packet packet;

    packet << "move" << source << target << army;

    return packet;
}

// send move
sf::Packet ServerCommandMove(int source, int sourceRemaining, int target, int targetRemaining, int targetOwner)
{
    sf::Packet packet;

    packet << "move" << source << sourceRemaining << target << targetRemaining << targetOwner;

    return packet;
}

// request add
sf::Packet ClientRequestAdd(int target, int army)
{
    sf::Packet packet;

    packet << "add" << target << army;

    return packet;
}

// send add
sf::Packet ServerCommandAdd(int target, int army, int owner)
{
    sf::Packet packet;

    packet << "add" << target << army << owner;

    return packet;
}
