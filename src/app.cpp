#include "app.hpp"
#include "editor.hpp"

#include <iostream>

App::App() {
	window.create(sf::VideoMode(
		sf::Vector2u(GRID_WIDTH + 1, GRID_HEIGHT) * static_cast<unsigned int>(SCALE * TILESIZE)), 
		"Vector Bound Level Editor", sf::Style::Close);
}

void App::run() {
	Editor editor(*this);

	sf::Clock clock;
	sf::Time deltaTime;

	while (window.isOpen()) {
		while (auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
			editor.onEvent(*event, window);
		}

		deltaTime = clock.restart();

		editor.update(deltaTime, window);

		window.clear(sf::Color(70, 70, 70));

		editor.draw(window);

		window.display();
	}
}