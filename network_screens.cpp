#include "network_screens.hpp"


//sf::Font loadFont(std::string path)
//{
//	sf::Font font;
//	if (!font.loadFromFile(path))
//	{
//		// error
//		exit(EXIT_FAILURE);
//	}
//
//	return font;
//}
//
//bool loadImages(World *world)
//{
//	std::string filename{ "" };
//	std::ifstream file("borderImagesList.txt");
//
//	for (int i = 0; file >> filename; i++) {
//		world->getTerritory(i)->borderTexture = sf::Texture();
//		if (!world->getTerritory(i)->borderTexture.loadFromFile(filename))
//			return 0;
//	}
//
//	file = std::ifstream("territoryImagesList.txt");
//
//	for (int i = 0; file >> filename; i++) {
//		world->getTerritory(i)->territoryTexture = sf::Texture();
//		if (!world->getTerritory(i)->territoryTexture.loadFromFile(filename))
//			return 0;
//	}
//
//	return 1;
//}
