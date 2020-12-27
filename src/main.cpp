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

/*
 * TODO
 * Try to update dependencies
 * Recompile SFML (and Allegro) as Release
 * Make each world a seperate level (Like acts in Sonic)
 * Add some sort of warning 'Debug Mode' text
 * Implement logger better (Or just use the console)
 * Try to compile SFML without some deps or compile source with engine
 * Move all functions to CrowEngine.cpp (but leave the headers in .hpp)
 * TGA (or even GIF) compresses better than PNG in zip files
 * Probably fix level memory leak by not using 'new' for scenes
 * 2 sound mixers, one for music, one for sfx?
 * BUG: Always use sizeof() since sizeof is VS exclusive
 */

/*
 * TODO for release
 * 
 * MAYBE: If completed game and resuming, just show end screen
 * MAYBE: Add option to change used controller
 * MAYBE: Fix always ducking with mega drive controller (Can this be fixed?)
 * MAYBE: Fix transition not transitioning properly
 * MAYBE: Fix transition not working correctly after vieweing EndScreen more than once
 * 
 * 
 * Final
 * Updated the last stage
 * Minor fixes
 * 
 * FIXED: Updated spikes on 2-4
 * FIXED: Fixed water tiles not being affected by darkness
 * FIXED: Fixed 20px line not being covered when transitioning from level
 * FIXED: Made transitions not jump as much in the end
 * FIXED: Add more lava to 2-B
 * FIXED: Fixed wrong tile on 2-4
 * 
 * RC3
 * Remade and much improved jumping
 * Updated various levels
 * Minor controller fixes
 * Fixed rare crash when switching levels
 * Minor fixes
 * 
 * FIXED: Added water at the end of the fall in 2-4
 * FIXED: Fixed deadzone not working correctly on the X axis
 * FIXED: Reverted back to the old icon
 * FIXED: No longer checks for axis pos if controller isn't connected
 * FIXED: Renamed 'main' submenu to 'home' to avoid issues with main()
 * FIXED: Reworked jumping to be more responsive
 * FIXED: Reverted back to old layout of 2-B
 * ADDED: Animate boss health bar
 * FIXED: No longer assumed item is second fixture
 * FIXED: Fixed rare crash when switching levels
 * FIXED: Removed pointer to old controller scene
 * 
 * RC2
 * Controller support
 * Cleaned up boss sprite
 * Added a total of 100 coins
 * Updated various levels
 * Minor fixes
 * 
 * ADDED: Controller support
 * FIXED: Removed debug text in submenu
 * FIXED: Removed key rebinding options
 * FIXED: Fixed not saying time when completing final boss
 * FIXED: Remake enemy.tga
 * ADDED: Display coins, total time and deaths on end screen
 * FIXED: Removed unusued santa sprite
 * FIXED: Removed logging of some joystick info
 * FIXED: Fixed some wrong tiles in 2-4 and 2-2
 * ADDED: Added a total of 100 coins
 * FIXED: Fixed seeing feet under water
 * FIXED: Updated 2-2, 1-1, 2-4, 2-3, 1-B
 */

/*
 * Cleaned up scenes
 * SceneEndGame		Good
 * SceneHUD			Clean
 * SceneLevel		Clean
 * SceneMenu		Clean
 * SceneSubMenu		Cleaned
 * 
 * Clean:	Not cleaned yet
 * Cleaned:	Unused vars cleaned
 * Good:	Unused cars, unused code and todos cleaned
 */

/*
 * Taco size
 * taco is ~57 kb smaller if using 7z
 * engine is ~11 kb larger (~6 kb compressed)
 */

/*
 * Build Types
 * Allegro	Release		(Release is ~65 kb larger, ~10 kb compressed)
 * SFML		MinSizeRel	(Release is ~66 kb larger, ~22 kb compressed)
 * Deps		Release
 */

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