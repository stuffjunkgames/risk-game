#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

int RunServer();
void SendAllClients(sf::Packet &packet, std::vector<std::unique_ptr<sf::TcpSocket>> &clients);

#endif // SERVER_HPP_INCLUDED