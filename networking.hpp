#ifndef NETWORKING_HPP_INCLUDED
#define NETWORKING_HPP_INCLUDED

#include <SFML/Network.hpp>
#include "movement.hpp"

// Move functions - used for "attack" and "reposition" phases
// request move
sf::Packet ClientRequestMove(int source, int target, int army);

// send move
sf::Packet ServerCommandMove(int source, int sourceRemaining, int target, int targetRemaining, int targetOwner);


// Add functions - used for game-start and "place" phase
// request add
sf::Packet ClientRequestAdd(int target, int army);

// send add
sf::Packet ServerCommandAdd(int target, int remaining, int owner);


// request the phase and current player
sf::Packet ClientRequestPhaseChange();

// send the current phase and current player
sf::Packet ServerCommandPhaseChange(TurnPhase phase, int player);


// request player ready
sf::Packet ClientRequestReady(std::string name, sf::Color color);

// send player configurations
sf::Packet ServerCommandReady(int playerID, std::string name, sf::Color color);


// send player ID, only to that player
sf::Packet ServerCommandID(int playerID);

// syncronizing messages in chat
sf::Packet ClientRequestMessage(std::string message);

sf::Packet ServerCommandMessage(int playerID, std::string message);

// sending exit signal
sf::Packet ClientRequestResign();

#endif // NETWORKING_HPP_INCLUDED
