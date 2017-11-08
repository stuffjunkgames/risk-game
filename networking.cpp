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

sf::Packet ClientRequestPhaseChange()
{
	sf::Packet packet;

	packet << "phase";

	return packet;
}

sf::Packet ServerCommandPhaseChange(TurnPhase phase, int player)
{
	sf::Packet packet;

	packet << "phase" << phase << player;

	return packet;
}

// request player ready
sf::Packet ClientRequestReady(std::string name, sf::Color color)
{
	sf::Packet packet;

	packet << "ready" << name << color.toInteger();

	return packet;
}

// send player configurations
sf::Packet ServerCommandReady(int playerID, std::string name, sf::Color color)
{
	sf::Packet packet;

	packet << "ready" << playerID << name << color.toInteger();

	return packet;
}

// send player ID, only to that player
sf::Packet ServerCommandID(int playerID)
{
    sf::Packet packet;

    packet << "id" << playerID;

    return packet;
}

sf::Packet ClientRequestMessage(int playerID, std::string message)
{
	sf::Packet packet;

	packet << "chat" << playerID << message;

	return packet;
}

sf::Packet ServerCommandMessage(int playerID, std::string message)
{
	sf::Packet packet;

	packet << "chat" << playerID << message;

	return packet;
}