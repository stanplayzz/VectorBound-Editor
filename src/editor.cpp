#include "editor.hpp"

#include <djson/json.hpp>

#include <Windows.h>
#include <ShObjIdl.h>

#include <print>
#include <iostream>
#include <exception>

Editor::Editor(App& app) : app(app), tileManager(app), hover(tileManager.tileset) {
	hover.setTextureRect(sf::IntRect({ 0 * app.TILESIZE, 0 }, { app.TILESIZE, app.TILESIZE }));
	hover.setScale(sf::Vector2f(app.SCALE, app.SCALE));
	hover.setColor(sf::Color(255, 255, 255, 100));
	loadGrid();
	loadHotbar();
}

std::filesystem::path getAppFolder() {
#ifdef _WIN32
	const char* docPath = std::getenv("USERPROFILE");
	std::filesystem::path base = docPath ? docPath : "";
	base /= "Documents/VectorBound";
#else
	const char* docPath = std::getenv("HOME");
	std::filesystem::path base = docPath ? docPath : "";
	base /= "Documents/VectorBound";
#endif

	std::filesystem::create_directories(base);
	return base;
}

std::filesystem::path pickFolder() {
	auto hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to initialize COM");
	}

	IFileDialog* pfd = nullptr;
	hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to initialize instance");
	}

	DWORD options;
	pfd->GetOptions(&options);
	pfd->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

	std::filesystem::path result;
	if (SUCCEEDED(pfd->Show(nullptr))) {
		IShellItem* psiResult = nullptr;
		if (SUCCEEDED(pfd->GetResult(&psiResult))) {
			PWSTR folderPath = nullptr;
			if (SUCCEEDED(psiResult->GetDisplayName(SIGDN_FILESYSPATH, &folderPath))) {
				result = folderPath;
				CoTaskMemFree(folderPath);
			}
			psiResult->Release();
		}
	}

	pfd->Release();
	CoUninitialize();

	return result;
}

void Editor::onEvent(sf::Event event, sf::RenderWindow& window) {
	if (auto key = event.getIf<sf::Event::KeyPressed>()) {
		if (key->code >= sf::Keyboard::Key::Num0 && key->code <= sf::Keyboard::Key::Num9) {
			int num = static_cast<int>(key->code) - static_cast<int>(sf::Keyboard::Key::Num0);
			if (num <= 5) {
				tileManager.currentTile = static_cast<TileType>(num);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
			saveToFile();

		}
	}
	if (auto button = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (button->button == sf::Mouse::Button::Left) {
			auto mousePos = sf::Mouse::getPosition(window);
			sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
			if (mousePos.x < app.GRID_WIDTH * app.TILESIZE * app.SCALE) {
				sf::Vector2i gridPos{
					int(worldPos.x / (app.TILESIZE * app.SCALE)),
					int(worldPos.y / (app.TILESIZE * app.SCALE))
				};

				tileManager.place(gridPos);

			}
			else {
				for (int i = 0; i < hotbar.size(); i++) {
					if (hotbar[i].getGlobalBounds().contains(worldPos)) {
						tileManager.currentTile = static_cast<TileType>(i);
						hover.setTextureRect(sf::IntRect({ i * app.TILESIZE, 0 }, { app.TILESIZE, app.TILESIZE }));
					}
				}
			}
		}
	}
}

void Editor::loadGrid() {
	const int width = app.GRID_WIDTH * app.TILESIZE * app.SCALE;
	const int height = app.GRID_HEIGHT * app.TILESIZE * app.SCALE;

	grid.setPrimitiveType(sf::PrimitiveType::Lines);

	for (int x = 0; x <= width; x += app.TILESIZE * app.SCALE ) {
		grid.append(sf::Vertex(sf::Vector2f(x, 0), sf::Color(200, 200, 200)));
		grid.append(sf::Vertex(sf::Vector2f(x, height), sf::Color(200, 200, 200)));
	}

	for (int y = 0; y <= height; y += app.TILESIZE * app.SCALE) { 
		grid.append(sf::Vertex(sf::Vector2f(0, y), sf::Color(200, 200, 200)));
		grid.append(sf::Vertex(sf::Vector2f(width, y), sf::Color(200, 200, 200)));
	}
}

void Editor::loadHotbar() {
	const float width = app.GRID_WIDTH * app.TILESIZE * app.SCALE;

	for (int i = 0; i <= 5; i++) {
		sf::Sprite sprite(tileManager.tileset);
		sprite.setTextureRect(sf::IntRect({ i * app.TILESIZE, 0 }, { app.TILESIZE, app.TILESIZE }));
		sprite.setScale(sf::Vector2f(app.SCALE, app.SCALE));
		sprite.setPosition(sf::Vector2f(width, i * app.SCALE * app.TILESIZE));

		hotbar.push_back(sprite);
	}
}

void Editor::draw(sf::RenderWindow& window) {
	window.draw(grid);
	window.draw(hover);
	for (auto& sprite : hotbar) {
		window.draw(sprite);
	}
	tileManager.draw(window);
}

void Editor::update(sf::Time deltaTime, sf::RenderWindow& window) {
	auto mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
	if (mousePos.x < app.GRID_WIDTH * app.TILESIZE * app.SCALE) {
		sf::Vector2i gridPos{
			int(worldPos.x / (app.TILESIZE * app.SCALE)),
			int(worldPos.y / (app.TILESIZE * app.SCALE))
		};

		hover.setPosition(sf::Vector2f(gridPos * app.TILESIZE * app.SCALE));
	}
	else {
		hover.setPosition(sf::Vector2f(10000.f, 10000.f));
	}
}

void Editor::saveToFile() {
	std::vector<int> level = tileManager.exportLevel();

	auto json = dj::Json::empty_object();
	json["Level"] = dj::Json::empty_array();
	for (auto tile : level) {
		json["Level"].push_back(tile);
	}

	auto folder = pickFolder();
	int index;
	std::print("Level Index: ");
	std::cin >> index;

	if (!json.to_file(std::format("{}/level{}.json", folder.string(), index))) {
		std::println("Failed to save level to JSON");
	}
	else {
		std::println("Succesfully saved the level to JSON");
	}
}