#include "tileManager.hpp"

#include <string>
#include <exception>
#include <print>

TileManager::TileManager(App& app) : app(app) {
	auto path = std::string(ASSETS_DIR) + "/textures/tileset.png";
	if (!tileset.loadFromFile(path)) {
		throw std::runtime_error("Failed to load tileset at: " + path);
	}

}

void TileManager::place(sf::Vector2i gridPosition) {
	sf::Sprite sprite(tileset);
	sprite.setTextureRect(sf::IntRect({ static_cast<int>(currentTile) * app.TILESIZE, 0 }, { app.TILESIZE, app.TILESIZE }));
	sprite.setScale(sf::Vector2f(app.SCALE, app.SCALE));
	sprite.setPosition(sf::Vector2f(gridPosition * app.TILESIZE * app.SCALE));

	placedTiles.erase(
		std::remove_if(placedTiles.begin(), placedTiles.end(),
			[&](const PlacedTile& tile) {
				return tile.sprite.getPosition() == sf::Vector2f(gridPosition * app.TILESIZE * app.SCALE);
			}),
		placedTiles.end()
	);

	placedTiles.push_back(PlacedTile{ sprite, static_cast<int>(currentTile) });
}

void TileManager::draw(sf::RenderWindow& window) {
	for (auto& tile : placedTiles) {
		window.draw(tile.sprite);
	}
	sf::Sprite currentTileSprite(tileset);
	currentTileSprite.setTextureRect(sf::IntRect({ static_cast<int>(currentTile) * app.TILESIZE, 0 }, { app.TILESIZE, app.TILESIZE }));
	currentTileSprite.setScale(sf::Vector2f(app.SCALE, app.SCALE));

	currentTileSprite.setPosition(sf::Vector2f(app.GRID_WIDTH * app.SCALE * app.TILESIZE, 
		(app.GRID_HEIGHT - 1) * app.SCALE * app.TILESIZE));

	window.draw(currentTileSprite);
}

std::vector<int> TileManager::exportLevel() const {
	std::vector<int> level(app.GRID_WIDTH * app.GRID_HEIGHT, 0);

	for (const auto& tile : placedTiles) {
		sf::Vector2f pos(tile.sprite.getPosition());
		int gridX = static_cast<int>(pos.x / (app.TILESIZE * app.SCALE));
		int gridY = static_cast<int>(pos.y / (app.TILESIZE * app.SCALE));

		if (gridX >= 0 && gridX < app.GRID_WIDTH && gridY >= 0 && gridX < app.GRID_HEIGHT) {
			level[gridY * app.GRID_WIDTH + gridX] = tile.id;
		}
	}

	return level;
}