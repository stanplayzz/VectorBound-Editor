#pragma once

#include <SFML/graphics.hpp>

class App {
public:
	App();
	void run();

	int GRID_WIDTH = 8;
	int GRID_HEIGHT = 8;
	int SCALE = 4;
	int TILESIZE = 16;

	float TITLEBARHEIGHT = 80.f;
private:
	sf::RenderWindow window;
	sf::View view;
};