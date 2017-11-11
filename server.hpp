#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

class ClientsList
{
	std::vector<std::shared_ptr<sf::TcpSocket>> clients;
	std::vector<bool> readyStatus;
	std::vector<int> playerIDs;
	// last client you received a packet from
	int lastClient;

public:
	ClientsList();

	void AddClient(std::shared_ptr<sf::TcpSocket> socket);
	sf::Socket::Status ClientsList::ReceivePacket(sf::Packet &packet, int &id);
	bool Ready(int id);
	void SendAll(sf::Packet &packet);
};

int RunServer();
void SendAllClients(sf::Packet &packet, std::vector<std::unique_ptr<sf::TcpSocket>> &clients);


#endif // SERVER_HPP_INCLUDED
