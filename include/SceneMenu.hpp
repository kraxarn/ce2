#pragma once

#include "CrowEngine.hpp"
#include "SceneSubMenu.hpp"

#include <SFML/Graphics/Font.hpp>
#include <functional>
#include "SceneLevel.hpp"

using namespace std;
using namespace ce;
using namespace sf;

class SceneMenu : public Scene
{
	enum Direction { right, left };
	//enum MenuItem  { newGame, options, exitGame };

	// Moved to SubMenu
	//enum Menu { home, options, controller, keyboard, soundtest };
	//Menu menu;

	Game *game;

	Font fontMenu, fontDebug;
	Text textMenu, textDebug, textTitle;

	RenderWindow *window;
	View *view;

	RectangleShape shapeBorder;

	Texture   texArrow, texDebug, texTree;
	Sprite    sprArrow, sprDebug, sprTree;
	Direction dirArrow;

	int cur;

	// Transitions
	RectangleShape rectTrans;
	int transPos;
	bool goToLevel;
	float transSpeed;

	Input input;

	// Keybindings
	//Keyboard::Key keyUp, keyDown, keyEnter, keyBack;

public:
	SceneMenu(Game *game)
	{
		#pragma region Local references
		this->game   = game;
		this->window = game->window->getWindow();
		this->view   = game->window->getView();
		#pragma endregion 

		#pragma region Textures and sprites
		game->fileStream->open("image/tree.tga");
		if (!texTree.loadFromStream(*game->fileStream))
			Engine::error("Failed to load tree image");

		game->fileStream->open("image/arrow.tga");
		if (!texArrow.loadFromStream(*game->fileStream))
			Engine::error("Failed to load arrow image");

		sprArrow.setTexture(texArrow);
		sprArrow.setPosition(76, 292);
		sprArrow.setOrigin(0, sprArrow.getLocalBounds().height / 2);
		
		sprTree.setTexture(texTree);
		sprTree.setPosition(1100, 720);
		sprTree.setOrigin(sprTree.getLocalBounds().width, sprTree.getLocalBounds().height);
		#pragma endregion 

		#pragma region Fonts and text
		if (!Util::loadFont(&fontMenu, "menu.ttf"))
			Engine::error("Failed to load menu.ttf");

		if (!Util::loadFont(&fontDebug, "debug.ttf"))
			Engine::error("Failed to load debug.ttf");

		Util::createText(
			&textMenu,
			"New Game\nLoad Game\nOptions\nExit Game",
			&fontMenu,
			32,
			Text::Regular,
			Color(255, 255, 255),
			Vector2f(128, 720 / 2)
		);
		textMenu.setOrigin(0, textMenu.getLocalBounds().height / 2);

		Util::createText(
			&textDebug,
			"",
			&fontDebug,
			16,
			Text::Regular,
			Color(255, 255, 255),
			Vector2f(16, 16)
		);

		Util::createText(
			&textTitle,
			"Kraxmas 2",
			&fontMenu,
			64,
			Text::Regular,
			Color::White,
			Vector2f(1216, 128)
		);
		textTitle.setOrigin(textTitle.getLocalBounds().width, 0);
		textTitle.setRotation(11.25f);
		#pragma endregion 

		#pragma region Debug: View border
		shapeBorder.setOutlineColor(Color::Red);
		shapeBorder.setOutlineThickness(4);
		shapeBorder.setFillColor(Color(0, 0, 0, 0));
		shapeBorder.setPosition(0, 0);
		shapeBorder.setSize(Vector2f(1280, 720));
		#pragma endregion 

		#pragma region Vars
		dirArrow = left;
		cur = 0;
		#pragma endregion 

		#pragma region Music
		if (!game->audio->playMusic("menu.xm"))
			Engine::error("Failed to load menu music");
		#pragma endregion 

		rectTrans.setSize(Vector2f(1280, 720));
		rectTrans.setFillColor(Color::Black);
		rectTrans.setPosition(-1280, 720);
		goToLevel = false;
		transPos = -1280;
		transSpeed = 0.f;
	}

	static bool isJoystickUp(const Event::JoystickMoveEvent ev)
	{
		if (ev.axis == Joystick::PovY)
			return ev.position > 90.f;
		if (ev.axis == Joystick::Y)
			return ev.position < -90.f;
		return false;
	}
	static bool isJoystickDown(const Event::JoystickMoveEvent ev)
	{
		if (ev.axis == Joystick::PovY)
			return ev.position < -90.f;
		if (ev.axis == Joystick::Y)
			return ev.position > 90.f;
		return false;
	}

	void onEvent(const Event ev) override
	{
		auto up = false, down = false, enter = false;

		if (ev.type == Event::KeyPressed)
		{
			switch (ev.key.code)
			{
				case Keyboard::Up:     up    = true; break;
				case Keyboard::Down:   down  = true; break;
				case Keyboard::Return: enter = true; break;
				default: break;
			}
		}
		else if (ev.type == Event::JoystickButtonPressed)
		{
			if (ev.joystickButton.button == 0)
				enter = true;
		}
		else if (ev.type == Event::JoystickMoved)
		{
			if (isJoystickUp(ev.joystickMove))
				up = true;
			else if (isJoystickDown(ev.joystickMove))
				down = true;
		}

		if (up && cur > 0)
		{
			if (cur == 2 && !Engine::fileExists("save"))
			{
				sprArrow.move(0, -96);
				cur -= 2;
			}
			else
			{
				sprArrow.move(0, -48);
				cur--;
			}
		}
		else if (down && cur < 3)
		{
			if (cur == 0 && !Engine::fileExists("save"))
			{
				sprArrow.move(0, 96);
				cur += 2;
			}
			else
			{
				sprArrow.move(0, 48);
				cur++;
			}
		}
		else if (enter)
		{
			switch (cur)
			{
				case 0:
					// Restore save
					delete game->save;
					game->save = new Save();
					game->hud->setCoins(0);
					// Load level
					goToLevel = true;
					break;
				case 1:
					// Load out current save
					if (!Engine::fileExists("save"))
						Engine::warning("No save to load");
					else
					{
						delete game->save;
						game->save = new Save("save");
						game->hud->setCoins(game->save->coins);
						goToLevel = true;
					}
					break;
				case 2:
					view->reset(FloatRect(-1280, 720, 1280, 720));
					game->currentScene = game->scenes.submenu;
					break;
				case 3:
					// Do this fancier?
					window->close();
					exit(0);
				default: break;
			}
		}
	}
	void render(const Time frameTime) override
	{
		#pragma region Move arrow
		// 0 - 10

		// Set offset
		auto offArrow = (sprArrow.getPosition().x - 82);
		if (offArrow < 0)
			offArrow = -offArrow;

		// Move arrow
		if (dirArrow == left)
			sprArrow.move(-0.5 - (offArrow / 10.0), 0);
		else
			sprArrow.move(0.5 + (offArrow / 10.0), 0);

		// Decide if we need to change direction
		if (sprArrow.getPosition().x <= 64)
			dirArrow = right;
		else if (sprArrow.getPosition().x >= 82)
			dirArrow = left;
		#pragma endregion 

		// Update debug text
		textDebug.setString(fmt::format("Cur: {}\nFPS: {} \nDelta: {}", cur, 1000000 / static_cast<int>(frameTime.asMicroseconds()), frameTime.asMilliseconds()));

		#pragma region Transition to main game
		if (goToLevel)
		{
			// Fix for right stripe at end of transition
			// Not pretty, but it works
			// Could also be -19
			if (transPos == 24)
				transPos = 0;

			rectTrans.setPosition(transPos, 0);

			if (transPos <= 0)
			{
				transPos += transSpeed;
				transSpeed += 0.75;
			}
			else
			{
				//delete game->scenes.level;
				//game->scenes.level = new SceneLevel(game);

				game->currentScene = game->scenes.level;
				static_cast<SceneLevel*>(game->currentScene)->resetLevel();
				static_cast<SceneLevel*>(game->currentScene)->enter();
				goToLevel = false;
			}
		}
		else if (rectTrans.getPosition().x != -1280)
		{
			// Not pretty, but it works
			// This is for when going back to the menu from level
			// Mostly to avoid the flash since we clear and render after this
			rectTrans.setPosition(-1280, 720);
			transPos = -1280;
			transSpeed = 0.f;
		}
		#pragma endregion

		#pragma region Draw everything
		// Clear
		window->clear(Color(51, 153, 218));

		// This is mostly to fix going back from level


		// Debug: Draw border
		//window->draw(shapeBorder);

		// Draw text
		window->draw(textMenu);
		window->draw(textTitle);
		if (game->devMode)
			window->draw(textDebug);

		// Draw arrow
		window->draw(sprArrow);
		window->draw(sprTree);
		window->draw(sprDebug);

		window->draw(rectTrans);
		#pragma endregion

		//cout << Joystick::getAxisPosition(0, Joystick::PovX) << endl;
	}

	bool loadSave()
	{
		// Load file
		fstream saveFile(Engine::getWritePath("save"));
		// Check if it could be loaded
		if (!saveFile.good())
			return false;
		// If it could be, load it
		saveFile.close();
		delete game->save;
		game->save = new Save(Engine::getWritePath("save"));
		// Go to level
		goToLevel = true;
		// It's probably fine
		return true;
	}
};