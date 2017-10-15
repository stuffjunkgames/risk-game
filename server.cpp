#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>

// server code

int RunServer()
{
    // do server stuff...

    //////////// SETUP /////////////

    unsigned int port = 12345;
    std::string address = "msquared169.ddns.net";

    // CLI configurations? (later)

    // create world
    //World world;

    // create listener
    sf::TcpListener listener;
    sf::Packet packet;
    if(listener.listen(port) != sf::Socket::Done)
    {
        std::cout << "Error listening to port!\n";
        return 1;
    }

    listener.setBlocking(false);

    std::vector<sf::TcpSocket> clients;
    bool started = false;

    // listen for client connections
    // wait for start signal from clients (all, just host, something else?)
    while(!started)
    {
        sf::TcpSocket client;
        if(listener.accept(client) == sf::Socket::Status::Done)
        {
            //clients.push_back(client);

            // generate player number and send to all connected clients
        }

        for(unsigned int i = 0; i < clients.size(); i++)
        {
            sf::Packet packet;
            sf::Socket::Status status = clients[i].receive(packet);
            if(status == sf::Socket::Status::Done)
            {
                // open the packet
                std::string s;
                if(packet >> s)
                {
                    if(s == "start")
                    {
                        started = true;
                    }
                }
            }
        }

    }


    // send configurations to clients? (later)


    // assign territories to players



    // send assignments to clients


    // pick starting player, send to clients
    int startPlayer = rand() % clients.size() + 1;


    //////////// GAMEPLAY ///////////
    // Wait for commands from current player client
    // check legality
    // send movements to all clients
    // send turn changes to all clients, including next player


}
