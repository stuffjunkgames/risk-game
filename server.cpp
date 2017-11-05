#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"
#include "server.hpp"

// only included for loadFont
#include "network_screens.hpp"

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
	std::vector<bool> readyStatus;
    bool started = false;

    // listen for client connections
    // wait for start signal from clients (all, just host, something else?)
    while(!started)
    {
        clients.push_back(std::unique_ptr<sf::TcpSocket>());
		readyStatus.push_back(false);
        if(listener.accept(*clients.back()) == sf::Socket::Status::Done)
        {
            std::cout << "Player connected! " << clients.size() << " players have connected\n";

            // generate player number and send to all connected clients

            // add player to world
        }
        else
        {
            clients.pop_back();
			readyStatus.pop_back();
        }
		if (clients.size() > 0)
		{
			started = true;
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
                    if(s == "ready")
                    {
						// get configurations from player (name, color, etc.)
						readyStatus[i] = true;
						std::string name;
						sf::Uint32 color;
						int id;
						if (packet >> name >> color)
						{
							id = world.addPlayer(name, sf::Color(color));
						}
                        std::cout << "Player " << i << " is ready :P" << std::endl;
                        sf::Packet sendPacket = ServerCommandID(id);
                        clients[i].get()->send(sendPacket);
                    }
                }
            }
			if (!readyStatus[i])
			{
				started = false;
			}
        }
    }

	for (int i = 0; i < world.PlayerNumber(); i++)
	{
		int id = world.getPlayer(i)->getID();
		std::string name = world.getPlayerID(id)->getName();
		sf::Color color = world.getPlayerID(id)->getColor();
		sf::Packet packet = ServerCommandReady(id, name, color);
		SendAllClients(packet, clients);
	}

    // assign territories to players
    world.allocateTerritories();
    for(int i = 0; i < world.TerritoryNumber(); i++)
    {
        packet = ServerCommandAdd(i, world.getTerritory(i)->GetArmies(), world.getTerritory(i)->GetOwner()->getID());

		SendAllClients(packet, clients);
    }

    // pick starting player, send to clients
    //int currentPlayer = rand() % clients.size() + 1;
	int currentPlayer = world.getNextPlayer()->getID();
	packet = ServerCommandPhaseChange(TurnPhase::place, currentPlayer);
	SendAllClients(packet, clients);


    //////////// GAMEPLAY ///////////
    // Wait for commands from current player client
    // check legality
    // send movements to all clients
    // send turn changes to all clients, including next player

	TurnPhase phase = place;
	int armyCount = 3 + world.GetBonus(currentPlayer);
	int placedArmies = 0;
	World initialWorld = world;
	while (started)
	{
		// receive packet from the current player
		sf::Packet receivePacket, sendPacket;
		sf::Socket::Status status = clients[currentPlayer].get()->receive(receivePacket);

		// do something with the packet
		if (status == sf::Socket::Status::Done)
		{
			// open the packet
			std::string s;

			if (receivePacket >> s)
			{
				if (s == "add" && phase == TurnPhase::place)// client is requesting to add armies
				{
					int target, army;
					if (receivePacket >> target >> army)
					{
						armyCount -= Place(&world, &initialWorld, world.getPlayerID(currentPlayer), target, army, armyCount);
						sendPacket = ServerCommandAdd(target, world.getTerritory(target)->GetArmies(), currentPlayer);
						SendAllClients(sendPacket, clients);
					}
				}
				else if (s == "move" && phase == TurnPhase::attack)// client is requesting to attack a territory
				{
					int source, target, army;
					if (receivePacket >> source >> target >> army)
					{
						Attack(&world, world.getPlayerID(currentPlayer), source, target, army);
						sendPacket = ServerCommandMove(source, world.getTerritory(source)->GetArmies(), target, world.getTerritory(target)->GetArmies(), world.getTerritory(target)->GetOwner()->getID());
						SendAllClients(sendPacket, clients);
					}
				}
				else if (s == "move" && phase == TurnPhase::reposition)// client is requesting to move armies
				{
					int source, target, army;
					if (receivePacket >> source >> target >> army)
					{
						Reposition(&world, &initialWorld, world.getPlayerID(currentPlayer), source, target, army);
						sendPacket = ServerCommandMove(source, world.getTerritory(source)->GetArmies(), target, world.getTerritory(target)->GetArmies(), world.getTerritory(target)->GetOwner()->getID());
						SendAllClients(sendPacket, clients);
					}
				}
				else if (s == "phase")
				{
					switch (phase)
					{
					case TurnPhase::place:
					{
						phase = TurnPhase::attack;
						break;
					}
					case TurnPhase::attack:
					{
						phase = TurnPhase::reposition;
						break;
					}
					case TurnPhase::reposition:
					{
						phase = TurnPhase::place;
						currentPlayer = world.getNextPlayer()->getID();
						armyCount = 3 + world.GetBonus(currentPlayer);
						break;
					}
					default:
					{
						break;
					}
					}
					sendPacket = ServerCommandPhaseChange(phase, currentPlayer);
					SendAllClients(sendPacket, clients);
					initialWorld = world;
				}
			}
		}
	}

    return 0;
}

void SendAllClients(sf::Packet &packet, std::vector<std::unique_ptr<sf::TcpSocket>> &clients)
{
	for (unsigned int i = 0; i < clients.size(); i++)
	{
		clients[i].get()->send(packet);
	}
}
