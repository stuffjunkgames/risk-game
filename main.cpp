#include "risk.hpp"
#include "sfvm.hpp"
#include "movement.hpp"
#include "screens.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1800
#define GAME_HEIGHT 1012
#define GAME_RIGHT GAME_LEFT+GAME_WIDTH
#define GAME_BOTTOM GAME_TOP+GAME_HEIGHT
#define NO_KEY_PRESSED -1
#define KEY_PRESSED_ONCE -2
#define NO_BUTTON_PRESSED -1
#define BUTTON_PLUS 1
#define BUTTON_MINUS 2
#define BUTTON_ATTACK 3
#define BUTTON_CHANGE_PHASE 4
#define BUTTON_TEXTBOX 5


//////////////////////////////////////////////////////////////////////////////////////////////
// main

int main()
{
    sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Risk");

	sf::Color playerColor;
	std::string playerName;
	std::vector<sf::Color> playerColors;
	std::vector<std::string> playerNames;

	playerName = startScreen(&window, &playerColor);

	// This will be replaced with Networking handling getting other player names and colors

	playerNames.push_back("Blue Goo");
	playerColors.push_back(sf::Color(59, 160, 176, 255));// blue
	playerNames.push_back("Red Dread");
	playerColors.push_back(sf::Color(176, 59, 110, 255));// red
	playerNames.push_back("Green Spleen");
	playerColors.push_back(sf::Color(114, 181, 60, 255));// green


	if (playerName != "")
	{
		playerNames.push_back(playerName);
		playerColors.push_back(playerColor);
		gameScreen(&window, playerNames, playerColors);
	}

    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////////////////

// Returns a color that complements the given color
sf::Color complement(sf::Color color)
{
	sf::Color comp;
	double rp, gp, bp, Cmax, Cmin, delta, H, S, V, compH, C, X, m;

	rp = color.r / 255.0;
	gp = color.g / 255.0;
	bp = color.b / 255.0;
	Cmax = std::max(rp, std::max(gp, bp));
	Cmin = std::min(rp, std::max(gp, bp));
	delta = Cmax - Cmin;

	if (delta == 0) {
		H = 0;
	}
	else if (Cmax == rp) {
		H = 60.0 * (int((gp - bp) / delta) % 6);
	}
	else if (Cmax == gp) {
		H = 60.0 * (((bp - rp) / delta) + 2);
	}
	else if (Cmax == bp) {
		H = 60.0 * (((rp - gp) / delta) + 4);
	}

	if (Cmax == 0) {
		S = 0;
	}
	else {
		S = delta / Cmax;
	}

	V = Cmax;

	compH = int(H + 180) % 360;

	C = V * S;
	X = C * (1 - std::abs(int(compH / 60) % 2 - 1));
	m = V - C;
	if (compH >= 0 && compH < 60) {
		rp = C;
		gp = X;
		bp = 0;
	}
	else if (compH >= 60 && compH < 120) {
		rp = X;
		gp = C;
		bp = 0;
	}
	else if (compH >= 120 && compH < 180) {
		rp = 0;
		gp = C;
		bp = X;
	}
	else if (compH >= 180 && compH < 240) {
		rp = 0;
		gp = X;
		bp = C;
	}
	else if (compH >= 240 && compH < 300) {
		rp = X;
		gp = 0;
		bp = C;
	}
	else if (compH >= 300 && compH < 360) {
		rp = C;
		gp = 0;
		bp = X;
	}

	comp.r = (rp + m) * 255;
	comp.g = (gp + m) * 255;
	comp.b = (bp + m) * 255;

	return comp;
}
