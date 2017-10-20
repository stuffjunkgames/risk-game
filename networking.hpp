#ifndef NETWORKING_HPP_INCLUDED
#define NETWORKING_HPP_INCLUDED

#include <SFML/Network.hpp>

// request move
sf::Packet ClientRequestMove(int source, int target, int army);

// send move
sf::Packet ServerCommandMove(int source, int sourceRemaining, int target, int targetRemaining, int targetOwner);

// request add
sf::Packet ClientRequestAdd(int target, int army);

// send add
sf::Packet ServerCommandAdd(int target, int army, int owner);


#endif // NETWORKING_HPP_INCLUDED
