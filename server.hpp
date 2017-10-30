#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

int RunServer();
void SendAllClients(sf::Packet &packet, std::vector<std::unique_ptr<sf::TcpSocket>> &clients);

#endif // SERVER_HPP_INCLUDED