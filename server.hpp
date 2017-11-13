#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

class ClientsList
{

public:
	ClientsList();

	void AddClient(std::shared_ptr<sf::TcpSocket> socket);
	sf::Socket::Status ReceivePacket(sf::Packet &packet, int &id);
	bool Ready(int id);
	void SendAll(sf::Packet &packet);
	void ResignPlayerID(int id);
	void SkipResignedClients(World &world, int &id);
	int CheckForWinner();

	std::vector<std::shared_ptr<sf::TcpSocket>> clients;
	std::vector<bool> readyStatus;
	std::vector<int> playerIDs;
	std::vector<int> resignedClients;
	// last client you received a packet from
	int lastClient;
};

int main();

void SendAllClients(sf::Packet &packet, std::vector<std::unique_ptr<sf::TcpSocket>> &clients);


#endif // SERVER_HPP_INCLUDED
