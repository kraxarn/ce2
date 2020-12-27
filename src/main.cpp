// C++
#include <iostream>
#include <fstream>

// SFML
#include <SFML/Graphics.hpp>

// Libraries
#include "fmt/format.h" // ~18 KB

// Engine
#include "CrowEngine.hpp"
#include "CrowLevel.hpp"
#include "CrowAudio.hpp"

// Scenes
// Include all scenes here as 'workaround'
#include "SceneMenu.hpp"
#include "SceneLevel.hpp"
#include "SceneHUD.hpp"
#include "SceneEndGame.hpp"

// Namespace
using namespace std;
using namespace sf;
using namespace ce;

int main(int argc, char *argv[])
{
	#pragma region Check Level size
	// Semi-debug: Just to make sure it loads correctly
	if (sizeof(Level::LevelFile) != 7568)
		Engine::error("Invalid size of level file");
	#pragma endregion

	#pragma region Game class
	Game game;
	#pragma endregion 

	#pragma region Game config
	Config config;
	game.config = &config;
	//game.devMode = true; // Change for release
	#pragma endregion 

	#pragma region Main window
	ce::Window windowManager(VideoMode(1280, 720), Vector2f(1280, 720), 60);
	game.window = &windowManager;
	// So we can access them easier
	auto *window = windowManager.getWindow();
	auto *view   = windowManager.getView();
	#pragma endregion

	#pragma region Developer mode
	// Force debug mode with shift
	if (Keyboard::isKeyPressed(Keyboard::LShift))
		game.devMode = true;
	#pragma endregion

	#pragma region Engine init
	Engine(game.devMode);
	#pragma endregion 

	#pragma region Audio manager
	Audio audioManager(config.musicVolume, config.soundVolume);
	game.audio = &audioManager;
	#pragma endregion

	//region Save
	#pragma region Save
	if (Engine::fileExists("save"))
		game.save = new Save("save");
	else
		game.save = new Save();
	#pragma endregion
	//endregion

	#pragma region Config file
	// New way with Config class directly
	if (!game.config->loadFromFile())
		Engine::log("No config file");
	else
	{
		// Update sound manager
		game.audio->setMusicVolume(config.musicVolume);
		game.audio->setSoundVolume(config.soundVolume);
		// Update window size
		game.window->setSize((float) config.windowScale);
	}
	#pragma endregion 

	#pragma region SFML PhysFS
	FileStream fileStream;
	game.fileStream = &fileStream;
	#pragma endregion

	#pragma region Levels
	// 2 worlds, 4 levels each + 1 boss, 10 levels
	game.levels[0] = new Level(LevelType::stage, "1-1.clf", "level1.it", "World 1 - Level 1");
	game.levels[1] = new Level(LevelType::stage, "1-2.clf", "level1.it", "World 1 - Level 2");
	game.levels[2] = new Level(LevelType::stage, "1-3.clf", "level1.it", "World 1 - Level 3");
	game.levels[3] = new Level(LevelType::stage, "1-4.clf", "level1.it", "World 1 - Level 4");
	game.levels[4] = new Level(LevelType::boss,  "1-B.clf", "boss.xm",   "Boss");

	game.levels[5] = new Level(LevelType::stage, "2-1.clf", "level2.it", "World 2 - Level 1");
	game.levels[6] = new Level(LevelType::stage, "2-2.clf", "level2.it", "World 2 - Level 2");
	game.levels[7] = new Level(LevelType::stage, "2-3.clf", "level2.it", "World 2 - Level 3");
	game.levels[8] = new Level(LevelType::stage, "2-4.clf", "level2.it", "World 2 - Level 4");
	game.levels[9] = new Level(LevelType::boss,  "2-B.clf", "boss.xm",   "Final Boss");
	//game.levels = levels;
	#pragma endregion

	#pragma region Scene stuff
	game.hud            = new SceneHUD(&game);
	game.scenes.menu    = new SceneMenu(&game);
	game.scenes.submenu = new SceneSubMenu(&game);
	game.scenes.level   = new SceneLevel(&game);
	game.scenes.endgame = new SceneEndGame(&game);
	game.currentScene   = game.scenes.menu;
	#pragma endregion

	#pragma region For delta time
	Clock frameClock;
	#pragma endregion

	// This is global across all scenes
	while (window->isOpen())
	{
		#pragma region Event handling
		Event ev;
		while (window->pollEvent(ev))
		{
			// We always want to handle close event, pass the rest to the current scene
			if (ev.type == Event::Closed)
				window->close();
			else
				game.currentScene->onEvent(ev);
		}
		#pragma endregion 

		#pragma region Window handling and drawing
		// Delta time
		const auto frameTime = frameClock.restart();

		// Default clear (Maybe remove?)
		window->clear();

		// Set view (Is this needed? (prob) / In scene?)
		window->setView(*view);

		// Render scene
		if (game.currentScene == nullptr)
			Engine::error("No scene set to render");
		game.currentScene->render(frameTime);

		// Draw
		window->display();
		#pragma endregion 
	}

	// Close
	return 0;
}