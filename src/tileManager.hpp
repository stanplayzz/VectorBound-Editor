#pragma once
#include "app.hpp"

#include <SFML/Graphics.hpp>

struct PlacedTile {
	sf::Sprite sprite;
	int id;
};

enum class TileType {
	Air = 0,
	Red = 1,
	Blue = 2,
	Green = 3,
	Yellow = 4,
	Unbreakable = 5
};

class TileManager {
public:
	TileManager(App& app);
	void place(sf::Vector2i gridPosition);
	void draw(sf::RenderWindow& window);
	std::vector<int> exportLevel() const;

	TileType currentTile{};
	sf::Texture tileset;
private:
	std::vector<PlacedTile> placedTiles;
	App& app;
};