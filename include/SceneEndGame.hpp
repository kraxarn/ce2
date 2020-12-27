#pragma once

#include "CrowEngine.hpp"

using namespace ce;

class SceneEndGame : public Scene
{
	Font fontTitle, fontInfo;
	Text textTitle, textInfo;

	Texture texSanta;
	Sprite  sprSanta;

	Game         *game;
	RenderWindow *window;
	View         *view;

	RectangleShape rectTrans;
	int transPos;
	float transSpeed;

public:
	SceneEndGame(Game* game)
	{
		this->game   = game;
		this->window = game->window->getWindow();
		this->view   = game->window->getView();

		#pragma region Text and font
		// If they loaded fine thus far, it should be fine
		Util::loadFont(&fontTitle, "menu.ttf");
		Util::loadFont(&fontInfo, "debug.ttf");

		Util::createText(
			&textTitle,
			"Merry Christmas and a Happy New Year!",
			&fontTitle,
			42,
			Text::Regular,
			Color::White,
			Vector2f(640, 128)
		);
		textTitle.setOrigin(textTitle.getLocalBounds().width / 2, textTitle.getLocalBounds().height);
		Util::createText(
			&textInfo,
			"null",
			&fontInfo,
			18,
			Text::Regular,
			Color::White,
			Vector2f(128, 160)
		);
		#pragma endregion

		game->fileStream->open("image/santa.tga");
		if (!texSanta.loadFromStream(*game->fileStream))
			Engine::error("Failed to load santa image");
		sprSanta.setTexture(texSanta);
		sprSanta.setPosition(1100, 720);
		sprSanta.setOrigin(sprSanta.getLocalBounds().width, sprSanta.getLocalBounds().height);

		rectTrans.setSize(Vector2f(1280, 720));
		rectTrans.setFillColor(Color::Black);
		rectTrans.setPosition(0, 0);
		transPos = 0;
		transSpeed = 44.25;
	}
	void onEvent(const Event ev) override
	{
		if (ev.type == Event::KeyPressed)
		{
			if (ev.key.code == Keyboard::Return || ev.key.code == Keyboard::Escape)
			{
				// Same as pressing M in a level
				// (Except we don't resetLevel())
				view->reset(FloatRect(0, 0, 1280, 720));
				game->currentScene = game->scenes.menu;
				game->audio->playMusic("menu.xm");
			}
		}
	}
	void render(const Time frameTime) override
	{
		// Clear
		window->clear(Color(51, 153, 218));

		// Update transition
		rectTrans.setPosition(transPos, 0);
		if (transPos < 1280)
		{
			transPos += transSpeed;
			transSpeed -= 0.75;
		}

		// Update text
		textInfo.setString(fmt::format("Thanks for playing my little game and saving christmas!\nHope you enjoyed it as much as I did making it.\n\nThis is my first game actually finished in my custom game engine.\nIt has been in development for about 2 years now, but there are still lots to fix before it's done...\nThe game itself took a few months to make.\nIt was also a fun project where I wanted to see if I could fit a game under 1 megabyte.\n\n\nCoins collected: {}/100\nTime spent: {}\nDeaths: {}\n\n\nPress Enter to return to the main menu.\n", game->save->coins, Util::formatTime(game->save->totalTime * 1000), game->save->totalDeaths));

		// Draw text
		window->draw(textTitle);
		window->draw(textInfo);
		window->draw(sprSanta);
		window->draw(rectTrans);
	}
};