#pragma once

#include "CrowEngine.hpp"
using namespace ce;

#include <SFML/Graphics/Font.hpp>
using namespace sf;

#include <fmt/format.h>

class SceneSubMenu : public Scene
{
	enum Menu { home, controller, keyboard, soundtest};
	Menu currentMenu;

	enum Direction { right, left };
	Game* game;
	
	RenderWindow *window;
	View *view;

	Font fontMenu;
	Text textMenu;

	Texture texArrow;
	Sprite  sprArrow;

	int cur;

	// Default text for all menus
	string defaultText[4];
	// Default position for arrow
	int arrowPos[4];
	int menuOptions[4];
	Vector2f menuOffset[4];

	// For sound test
	int curMusic = 0, curSound = 0;

	string musicName[6] = { "World 1",   "World 2",   "Boss",    "Menu",    "Credits"    };
	char* musicFile[6]  = { "level1.it", "level2.it", "boss.xm", "menu.xm", "credits.it" };

	string soundName[6] = { "Boss taking damage", "Coin collected", "Level complete", "Taking damage", "Jump", "Present collected" };

public:
	SceneSubMenu(Game *game)
	{
		#pragma region Pointers
		this->game = game;
		this->window = game->window->getWindow();
		this->view = game->window->getView();
		#pragma endregion

		#pragma region Textures and sprites
		game->fileStream->open("image/arrow.tga");
		if (!texArrow.loadFromStream(*game->fileStream))
			Engine::error("Failed to load arrow image");

		sprArrow.setTexture(texArrow);
		sprArrow.setPosition(0, 0);
		sprArrow.setOrigin(0, sprArrow.getLocalBounds().height / 2);
		#pragma endregion

		#pragma region Fonts and text
		Util::loadFont(&fontMenu, "submenu.ttf");

		Util::createText
		(
			&textMenu,
			"",
			&fontMenu,
			28,
			Text::Regular,
			Color(250, 250, 250),
			Vector2f(128, 720 / 2)
		);
		textMenu.setOrigin(0, textMenu.getLocalBounds().height / 2);
		#pragma endregion 

		#pragma region Vars
		cur = 0;
		currentMenu = home;
		#pragma endregion 

		#pragma region Setup values for menus
		// Set default text for menus
		//defaultText[home] = "Window size: 0x0\n\nMusic volume: 0%\nSound volume: 0%\n\nController\nKeyboard\nSound test\n\nGo back";
		// Default to home
		//textMenu.setString(defaultText[currentMenu]);
		// Set default cusor position
		arrowPos[home]       = 274;		// 226
		arrowPos[controller] = 258;
		arrowPos[keyboard]   = 306;
		arrowPos[soundtest]  = 290;
		// Set number of options we have (-1)
		menuOptions[home]       = 6;
		menuOptions[controller] = 7;
		menuOptions[keyboard]   = 4;
		menuOptions[soundtest]  = 3;
		// Set offset for all menus
		menuOffset[home]       = Vector2f(0, 720);
		menuOffset[controller] = Vector2f(0, 0);
		menuOffset[keyboard]   = Vector2f(0, 1440);
		menuOffset[soundtest]  = Vector2f(1280, 720);
		#pragma endregion 
	}

	void onEvent(const Event ev) override
	{
		if (ev.type == Event::KeyPressed)
		{
			// Shortcut
			auto *cfg = game->config;

			if (ev.key.code == Keyboard::Up && cur > 0)
			{
				switch (currentMenu)
				{
					case home:       switch (cur) { case 2: case 5: case 9: cur--; default: break; } break;
					case controller: switch (cur) { case 3: case 7:         cur--; default: break; } break;
					case keyboard:   switch (cur) { case 4:                 cur--; default: break; } break;
					case soundtest:  switch (cur) { case 3:                 cur--; default: break; } break;
				}
				cur--;
			}
			else if (ev.key.code == Keyboard::Down && cur < menuOptions[currentMenu])
			{
				switch (currentMenu)
				{
					case home:       switch (cur) { case 0: case 3: case 7: cur++; default: break; } break;
					case controller: switch (cur) { case 1: case 5:         cur++; default: break; } break;
					case keyboard:   switch (cur) { case 2:                 cur++; default: break; } break;
					case soundtest:  switch (cur) { case 1:                 cur++; default: break; } break;
				}
				cur++;
			}
			else if (ev.key.code == Keyboard::Return)
			{
				switch (currentMenu)
				{
					case home:
						switch (cur)
						{
							//case 5: currentMenu = controller; cur = 0;  break;
							//case 6: currentMenu = keyboard;   cur = 0;  break;
							case 5: currentMenu = soundtest;  cur = 0;  break;

							case 6:
								// Save config
								game->config->saveToFile();
								// Reset view and go back
								view->reset(FloatRect(0, 0, 1280, 720));
								game->currentScene = game->scenes.menu;
								cur = 0;
								break;
						}
						break;
					case controller:
						switch (cur)
						{
							case 7: currentMenu = home; cur = 0; break;
						}
						break;
					case keyboard:
						switch (cur)
						{
							case 4: currentMenu = home; cur = 0; break;
						}
						break;
					case soundtest:
						switch (cur)
						{
							case 0:
								if (!game->audio->playMusic(musicFile[curMusic]))
									Engine::warning("Failed to play music");
							break;
							case 1:
								if (!game->audio->playSound(static_cast<Audio::SoundFile>(curSound)))
									Engine::warning("Failed to play sound");
								break;
							case 3: currentMenu = home; break;
						}
				}
			}
			else if (ev.key.code == Keyboard::Left)
			{
				switch (currentMenu)
				{
					// Options
					case home:
						if (cur == 0 && cfg->windowScale > 0.75)
						{
							// Window res
							cfg->windowScale -= 0.25;
							game->window->setSize(cfg->windowScale);
							//window->setSize(Vector2u(1280 * cfg->windowScale, 720 * cfg->windowScale));
							//game->window->centerWindow();
						}
						else if (cur == 2 && cfg->musicVolume > 0)
						{
							// Music volume
							cfg->musicVolume -= 5;
							game->audio->setMusicVolume(cfg->musicVolume);
						}
						else if (cur == 3 && cfg->soundVolume > 0)
						{
							// Sound volume
							cfg->soundVolume -= 5;
							game->audio->setSoundVolume(cfg->soundVolume);
						}
						break;
					// Sound test
					case soundtest:
						if (cur == 0 && curMusic > 0)
							curMusic--;
						else if (cur == 1 && curSound > 0)
							curSound--;
						break;
				}
			}
			else if (ev.key.code == Keyboard::Right)
			{
				// Helper to we can't change to higher res than the monitor itself
				const auto monitor = VideoMode::getDesktopMode();

				switch (currentMenu)
				{
					// Options
					case home:
						if (cur == 0 && (monitor.width > 1280 * cfg->windowScale && monitor.height > 720 * cfg->windowScale))
						{
							// Window res
							cfg->windowScale += 0.25;
							game->window->setSize(cfg->windowScale);
							//window->setSize(Vector2u(1280 * cfg->windowScale, 720 * cfg->windowScale));
							//game->window->centerWindow();
						}
						else if (cur == 2 && cfg->musicVolume < 200)
						{
							// Music volume
							cfg->musicVolume += 5;
							game->audio->setMusicVolume(cfg->musicVolume);
						}
						else if (cur == 3 && cfg->soundVolume < 100)
						{
							// Sound volume
							cfg->soundVolume += 5;
							game->audio->setSoundVolume(cfg->soundVolume);
						}
						break;
					// Sound test
					case soundtest:
						if (cur == 0 && curMusic < 4)
							curMusic++;
						else if (cur == 1 && curSound < 5)
							curSound++;
						break;
				}
			}
		}
	}

	void render(const Time frameTime) override
	{
		#pragma region Update various stuff
		// Update root based on view
		const auto viewRoot = Vector2f(view->getCenter().x - view->getSize().x / 2, view->getCenter().y - view->getSize().y / 2);
		const auto menuRoot = menuOffset[currentMenu];

		// Update arrow
		sprArrow.setPosition(menuRoot.x + 76, menuRoot.y + (arrowPos[currentMenu] + cur * 32));
		#pragma endregion 

		#pragma region Update menu text
		// TODO: Reset text center every frame, prob not needed
		textMenu.setOrigin(0, textMenu.getLocalBounds().height / 2);

		// TODO: Update text position, prob not needed
		textMenu.setPosition(menuRoot.x + 128, menuRoot.y + 720 / 2);

		// See what we need to update the text with
		// TODO: We don't need to update every frame, only when changing value
		auto *cfg = game->config;

		switch (currentMenu)
		{
			case home:       textMenu.setString(fmt::format("Window size: {}x{}\n\nMusic volume: {}%\nSound volume: {}%\n\nSound test\nGo back", 1280 * cfg->windowScale, 720 * cfg->windowScale, cfg->musicVolume, cfg->soundVolume)); break;
			case controller: textMenu.setString(fmt::format("Update controllers\nChange controller: {}\n\nLeft: {}\nRight: {}\nJump: {}\n\nGo back", Joystick::getIdentification(0).name.toAnsiString(), cfg->controller.left, cfg->controller.right, cfg->controller.jump)); break;
			case keyboard:   textMenu.setString(fmt::format("Left: {}\nRight: {}\nJump: {}\n\nGo back", game->config->keyboard.left, game->config->keyboard.right, game->config->keyboard.jump)); break;
			case soundtest:  textMenu.setString(fmt::format("Music: {}\nSound: {}\n\nGo back\n\nPlaying {}: {} / {}", musicName[curMusic], soundName[curSound], game->audio->getCurrentMusic(), game->audio->getMusicPosition(), game->audio->getMusicLength())); break;
		}
		#pragma endregion

		#pragma region Make sure we are looking at the text
		if      (viewRoot.x > menuRoot.x + 24) view->move(-25, 0);
		else if (viewRoot.x < menuRoot.x)      view->move(25, 0);
		else if (viewRoot.y > menuRoot.y + 24) view->move(0, -25);
		else if (viewRoot.y < menuRoot.y)      view->move(0, 25);
		#pragma endregion

		#pragma region Draw stuff
		// Clear
		window->clear(Color(51, 153, 218));
		window->draw(textMenu);
		window->draw(sprArrow);
		#pragma endregion 
	}
};
