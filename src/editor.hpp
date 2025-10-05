#pragma once
#include "tileManager.hpp"
#include "app.hpp"

#include <SFML/Graphics.hpp>


#include <string>

class Editor {
public:
	Editor(App& app);
	void update(sf::Time deltaTime, sf::RenderWindow& window);
	void onEvent(sf::Event event, sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
	void saveToFile();

	TileManager tileManager;
private:
	App& app;

	sf::Sprite hover;

	sf::VertexArray grid;
	std::vector<sf::Sprite> hotbar;

	void loadGrid();
	void loadHotbar();

};