#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"

#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>

// only included for loadFont
#include "screens.hpp"

// server code

int RunServer()
{
    // do server stuff...

    //////////// SETUP /////////////

    unsigned int port = 12345;
    std::string address = "msquared169.ddns.net";

    // CLI configurations? (later)

    // create world
    sf::Font armyFont = loadFont("arial.ttf");
    World world(armyFont);

    // create listener
    sf::TcpListener listener;
    sf::Packet packet;
    if(listener.listen(port) != sf::Socket::Done)
    {
        std::cout << "Error listening to port!\n";
        return 1;
    }

    listener.setBlocking(false);

    std::vector<std::unique_ptr<sf::TcpSocket>> clients;
    bool started = false;

    // listen for client connections
    // wait for start signal from clients (all, just host, something else?)
    while(!started)
    {
        clients.push_back(std::unique_ptr<sf::TcpSocket>());
        if(listener.accept(*clients.back()) == sf::Socket::Status::Done)
        {
            //clients.push_back(client);

            // generate player number and send to all connected clients

            // add player to world
        }
        else
        {
            clients.pop_back();
        }

        for(unsigned int i = 0; i < clients.size(); i++)
        {
            sf::Packet packet;
            sf::Socket::Status status = clients[i].get()->receive(packet);
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
    world.allocateTerritories();
    for(int i = 0; i < world.TerritoryNumber(); i++)
    {
        sf::Packet packet = ServerCommandAdd(i, world.getTerritory(i)->GetArmies(), world.getTerritory(i)->GetOwner()->getID());

        // send packet to all clients
    }




    // send assignments to clients


    // pick starting player, send to clients
    int startPlayer = rand() % clients.size() + 1;


    //////////// GAMEPLAY ///////////
    // Wait for commands from current player client
    // check legality
    // send movements to all clients
    // send turn changes to all clients, including next player

    return 0;
}
