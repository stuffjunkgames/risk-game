#define SERVER

#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "networking.hpp"
#include "server.hpp"

// only included for loadFont
#include "network_screens.hpp"

// server code

int main()
{
    // do server stuff...

    //////////// SETUP /////////////

    unsigned int port = 12345;
    std::string address = "msquared169.ddns.net";

	// create listener
	sf::TcpListener listener;
	sf::Packet packet;
	if (listener.listen(port) != sf::Socket::Done)
	{
		std::cout << "Error listening to port!\n";
		return 1;
	}

	listener.setBlocking(false);

    // create world
    sf::Font armyFont = loadFont("arial.ttf");
    World world(armyFont);

    bool started = false;
	ClientsList clientsList;

    // listen for client connections
    // wait for start signal from clients (all, just host, something else?)
    std::cout << "Listening for players..." << std::endl;
    while(!started)
    {
		std::shared_ptr<sf::TcpSocket> client = std::make_shared<sf::TcpSocket>();

        if(listener.accept(*(client.get())) == sf::Socket::Status::Done)
        {
            std::cout << "Player connected!\n";
			clientsList.AddClient(client);
        }

		sf::Packet packet;
		int id;
		sf::Socket::Status status = clientsList.ReceivePacket(packet, id);
		// open the packet
		std::string s;
		if (status == sf::Socket::Status::Done && packet >> s)
		{
			if (s == "ready")
			{
				// get configurations from player (name, color, etc.)
				std::string name;
				sf::Uint32 color;
				if (packet >> name >> color)
				{
					id = world.addPlayer(name, sf::Color(color));
				}
				std::cout << "Player " << id << " is ready :P with name \"" << name << "\" and color \"" << std::to_string(color) << "\"" << std::endl;
				started = clientsList.Ready(id);
			}
		}
    }

    std::cout << "Sending player information to clients..." << std::endl;
	for (int i = 0; i < world.PlayerNumber(); i++)
	{
		int id = world.getPlayer(i)->getID();
		std::string name = world.getPlayerID(id)->getName();
		sf::Color color = world.getPlayerID(id)->getColor();
		sf::Packet packet = ServerCommandReady(id, name, color);
		clientsList.SendAll(packet);
		std::cout << "Player " << id << std::endl;
	}

    // assign territories to players
    std::cout << "Allocating territories..." << std::endl;
    world.allocateTerritories();
    for(int i = 0; i < world.TerritoryNumber(); i++)
    {
        packet = ServerCommandAdd(i, world.getTerritory(i)->GetArmies(), world.getTerritory(i)->GetOwner()->getID());

		clientsList.SendAll(packet);

		std::cout << "Territory " << i << "; Player " << world.getTerritory(i)->GetOwner()->getID() << std::endl;
    }

    // pick starting player, send to clients
    //int currentPlayer = rand() % clients.size() + 1;
	int currentPlayerID = world.getNextPlayer()->getID();
	packet = ServerCommandPhaseChange(TurnPhase::place, currentPlayerID);
	clientsList.SendAll(packet);

	std::cout << "Player " << currentPlayerID << " starts!" << std::endl;


    //////////// GAMEPLAY ///////////
    // Wait for commands from current player client
    // check legality
    // send movements to all clients
    // send turn changes to all clients, including next player

	TurnPhase phase = place;
	int armyCount = 3 + world.GetBonus(currentPlayerID);
	int placedArmies = 0;
	World initialWorld = world;
	int winner;
	bool winnerSent = false;
	while (started)
	{
		sf::Packet receivePacket, sendPacket;
		int id;

		// check win conditions (only one player/client left)
		winner = clientsList.CheckForWinner();
		if (winner >= 0 && !winnerSent)
		{
			sendPacket = ServerCommandWinner(clientsList.playerIDs.at(winner));
			clientsList.SendAll(sendPacket);
			winnerSent = true;
		}

		// receive packet from the current player
		sf::Socket::Status status = clientsList.ReceivePacket(receivePacket, id);

		// do something with the packet
		if (status == sf::Socket::Status::Done)
		{
			// open the packet
			std::string s;

			if (receivePacket >> s)
			{
				if (id == currentPlayerID)
				{
					if (s == "add" && phase == TurnPhase::place)// client is requesting to add armies
					{
						int target, army;
						if (receivePacket >> target >> army)
						{
							int placed = Place(&world, &initialWorld, world.getPlayerID(currentPlayerID), target, army, armyCount);
							armyCount -= placed;
							sendPacket = ServerCommandAdd(target, placed, currentPlayerID);
							clientsList.SendAll(sendPacket);
							std::cout << "Adding " << army << " units to territory " << target << std::endl;
							std::cout << "Territory " << target << " now has " << world.getTerritory(target)->GetArmies() << " units" << std::endl;
						}
					}
					else if (s == "move" && phase == TurnPhase::attack)// client is requesting to attack a territory
					{
						int source, target, army;
						if (receivePacket >> source >> target >> army)
						{
							Attack(&world, world.getPlayerID(currentPlayerID), source, target, army);
							sendPacket = ServerCommandMove(source, world.getTerritory(source)->GetArmies(), target, world.getTerritory(target)->GetArmies(), world.getTerritory(target)->GetOwner()->getID());
							clientsList.SendAll(sendPacket);

							std::cout << "Moving " << army << " from territory " << source << " to territory " << target << std::endl;
							std::cout << "Territory " << source << " now has " << world.getTerritory(source)->GetArmies() << " armies and territory " << target << " now has " << world.getTerritory(target)->GetArmies() << " armies." << std::endl;
							
							if (world.getTerritory(target)->GetOwner()->getNumTerritories() <= 0)
							{
								clientsList.ResignPlayerID(world.getTerritory(target)->GetOwner()->getID());
							}

						}
					}
					else if (s == "move" && phase == TurnPhase::reposition)// client is requesting to move armies
					{
						int source, target, army;
						if (receivePacket >> source >> target >> army)
						{
							Reposition(&world, &initialWorld, world.getPlayerID(currentPlayerID), source, target, army);
							sendPacket = ServerCommandMove(source, world.getTerritory(source)->GetArmies(), target, world.getTerritory(target)->GetArmies(), world.getTerritory(target)->GetOwner()->getID());
							clientsList.SendAll(sendPacket);

							std::cout << "Moving " << army << " from territory " << source << " to territory " << target << std::endl;
							std::cout << "Territory " << source << " now has " << world.getTerritory(source)->GetArmies() << " armies and territory " << target << " now has " << world.getTerritory(target)->GetArmies() << " armies." << std::endl;
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
							currentPlayerID = world.getNextPlayer()->getID();
							// skip players if they've resigned
							clientsList.SkipResignedClients(world, currentPlayerID);							
							armyCount = 3 + world.GetBonus(currentPlayerID);
							break;
						}
						default:
						{
							break;
						}
						}
						sendPacket = ServerCommandPhaseChange(phase, currentPlayerID);
						clientsList.SendAll(sendPacket);
						initialWorld = world;

						std::cout << "Changing phase to " << phase << " for player " << currentPlayerID << std::endl;
					}
				}

				if (s == "chat")
				{
					std::string message;
					if (receivePacket >> message)
					{
						sendPacket = ServerCommandMessage(id, message);
						clientsList.SendAll(sendPacket);

						std::cout << "Player " << id << " says: " << message << std::endl;
					}
				}
				else if (s == "resign")
				{
					std::cout << "Player " << id << " has resigned." << std::endl;
					// The player has exited, and their turn will be skipped for the rest of the game
					clientsList.ResignPlayerID(id);
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

ClientsList::ClientsList()
{	
	int lastClient = -1;
}

void ClientsList::AddClient(std::shared_ptr<sf::TcpSocket> socket)
{
	clients.push_back(socket);
	readyStatus.push_back(false);
	playerIDs.push_back(-1);

	clients.back().get()->setBlocking(false);
}

sf::Socket::Status ClientsList::ReceivePacket(sf::Packet &packet, int &id)
{
	for (unsigned int i = 0; i < clients.size(); i++)
	{
		sf::Socket::Status status = clients[i].get()->receive(packet);
		if (status == sf::Socket::Status::Done)
		{
			lastClient = i;
			id = playerIDs[i];
			return status;
		}
	}
	id = -1;
	return sf::Socket::Status::NotReady;
}

bool ClientsList::Ready(int id)
{
	playerIDs.at(lastClient) = id;
	readyStatus.at(lastClient) = true;

	sf::Packet sendPacket = ServerCommandID(id);
	clients.at(lastClient).get()->send(sendPacket);

	for (unsigned int i = 0; i < readyStatus.size(); i++)
	{
		if (readyStatus.at(i) == false)
		{
			return false;
		}
	}
	return true;
}

void ClientsList::SendAll(sf::Packet &packet)
{
	for (unsigned int i = 0; i < clients.size(); i++)
	{
		clients[i].get()->send(packet);
	}
}

void ClientsList::ResignPlayerID(int id)
{
	for (unsigned int i = 0; i < playerIDs.size(); i++)
	{
		if (playerIDs[i] == id)
		{
			resignedClients.push_back(i);
			break;
		}
	}	
}

void ClientsList::SkipResignedClients(World &world, int &id)
{
	int currentPlayerID = id;
	bool skipping = true;
	while (skipping)
	{
		skipping = false;
		for (int i = 0; i < resignedClients.size(); i++)
		{
			// Check if the player has resigned
			if (playerIDs.at(resignedClients.at(i)) == currentPlayerID)
			{
				// Skip to next player
				std::cout << "Skipping Player " << currentPlayerID << std::endl;
				currentPlayerID = world.getNextPlayer()->getID();
				skipping = true;
				break;
			}
		}
	}
	id = currentPlayerID;
}

// Returns index of winning client
int ClientsList::CheckForWinner()
{
	if (clients.size() - resignedClients.size() > 1)
	{
		// still at least 2 players
		return -1;
	}
	int total = clients.size() * (clients.size() + 1) / 2;
	for (unsigned int i = 0; i < resignedClients.size(); i++)
	{
		total -= resignedClients.at(i) + 1;
	}

	return total - 1;
}
