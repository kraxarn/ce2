#pragma once

#include "CrowEngine.hpp"

using namespace ce;

class SceneHUD
{
	Game*         game;
	RenderWindow* window;
	View*         view;

	int coins, objectives, numObjectives;

	Texture texHUD;
	Sprite sprCoin, sprPresent;

	Font font;
	Text textCoin, textPresent;
public:
	SceneHUD(Game *game)
	{
		this->game   = game;
		this->window = game->window->getWindow();
		this->view   = game->window->getView();

		// Default values
		coins         = 0;
		objectives    = 0;
		numObjectives = 0;

		#pragma region Textures and sprites
		game->fileStream->open("tileset/hud.tga");
		if (!texHUD.loadFromStream(*game->fileStream))
			Engine::error("Failed to load hud tileset");

		sprCoin.setTexture(texHUD);
		sprCoin.setTextureRect(IntRect(0, 0, 32, 32));

		sprPresent.setTexture(texHUD);
		sprPresent.setTextureRect(IntRect(32, 0, 32, 32));
		#pragma endregion

		#pragma region Font and text
		if (!Util::loadFont(&font, "submenu.ttf"))
			Engine::error("Failed to load HUD font");
		Util::createText
		(
			&textCoin,
			"00",
			&font,
			32,
			Text::Regular,
			Color::White,
			Vector2f(0, 0)
		);
		textCoin.setOutlineColor(Color(0, 0, 0));
		textCoin.setOutlineThickness(2);
		Util::createText
		(
			&textPresent,
			"00/00",
			&font,
			32,
			Text::Regular,
			Color::White,
			Vector2f(0, 0)
		);
		textPresent.setOutlineColor(Color(0, 0, 0));
		textPresent.setOutlineThickness(2);
		#pragma endregion 
	}
	void draw(const Vector2f root)
	{
		#pragma region Update strings
		textCoin.setString(fmt::format("{}", coins));
		textPresent.setString(fmt::format("{}/{}", objectives, numObjectives));
		#pragma endregion 

		#pragma region Update positions
		sprCoin.setOrigin(sprCoin.getLocalBounds().width, 0);
		sprPresent.setOrigin(sprPresent.getLocalBounds().width, 0);

		textCoin.setOrigin(textCoin.getLocalBounds().width, 0);
		textPresent.setOrigin(textPresent.getLocalBounds().width, 0);

		sprCoin.setPosition(root.x + 1264, root.y + 16);
		sprPresent.setPosition(root.x + 1264, root.y + 64);

		textCoin.setPosition(root.x + 1216, root.y + 10);
		textPresent.setPosition(root.x + 1216, root.y + 58);
		#pragma endregion

		#pragma region Draw everything
		window->draw(sprCoin);
		window->draw(textCoin);
		if (numObjectives > 0)
		{
			window->draw(sprPresent);
			window->draw(textPresent);
		}
		#pragma endregion 
	}

	void loadFromSave()
	{
		// Loads from the global save
		this->coins = game->save->coins;
	}

	bool allObjectivesCollected() const
	{ return objectives == numObjectives; }

	void updateCoins(const int change)
	{ this->coins += change; }
	void setCoins(const int amount)
	{ this->coins = amount; }
	void updatePresents(const int change)
	{ this->objectives += change; }
	void setPresents(const int amount)
	{ this->objectives = amount; }
	void setNumPresents(const int amount)
	{ this->numObjectives = amount; }

	int getCoins() const
	{
		return coins;
	}
};