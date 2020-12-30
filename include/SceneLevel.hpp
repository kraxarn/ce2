#pragma once

#include "CrowEngine.hpp"
#include "SceneHUD.hpp"
#include "CrowLevel.hpp"
#include "AnimatedSprite.hpp"
#include "CrowTimer.hpp"

#include <iostream>
#include <chrono>

#include <box2d/b2_world.h>

using namespace std;
using namespace chrono;

#pragma region Collectable items (Coins, presents)
enum ItemType { coin, objective };
class CollectableItem
{
public:
	ItemType type;
	int x, y;
	bool collected;
	int tileID;
	CollectableItem(const ItemType type, const int x, const int y)
	{
		this->type = type;
		this->x = x;
		this->y = y;
		this->collected = false;

		if (type == coin)
			this->tileID = 2;
		else if (type == objective)
			this->tileID = 3;
		else
			this->tileID = -1;
	}
	Vector2f getPosition() const
	{
		return Vector2f(this->x * 64, this->y * 64);
	}
};
// Collectable items
vector<CollectableItem> cItems;
#pragma endregion

class ForegroundTile
{
public:
	int x, y;
	int tileID;
	ForegroundTile(const int x, const int y, const int tileID)
	{
		this->x = x;
		this->y = y;
		this->tileID = tileID;
	}
	Vector2f getPosition() const
	{
		return Vector2f(this->x * 64, this->y * 64);
	}
};

class SceneLevel;

class LevelSwitcher
{
public:
	static void reset(Game* game);
};

#pragma region ContactListener
class ContactListener : public b2ContactListener
{
	Player**     player;
	Save**       save;
	SceneHUD**   hud;
	Audio**      audio;
	SceneLevel** level;

	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

public:
	ContactListener(Player** player, Game** game, SceneLevel** level)
	{
		this->player = player;

		this->save  = &(*game)->save;
		this->hud   = &(*game)->hud;
		this->audio = &(*game)->audio;

		this->level = level;
	}
};
#pragma endregion 

class SceneLevel : public Scene
{
	#pragma region Pointers
	Game         *game;
	RenderWindow *window;
	View         *view;
	// Moved so we can access it from contact listener
	Player *player;
	Boss   *boss;
	RectangleShape bossHealth;
	#pragma endregion

	#pragma region Fonts and text
	Font fontDebug, fontPaused;
	Text textInfo,  textPaused, textPausedInfo, textTitle;
	#pragma endregion

	#pragma region Level stuff
	Level::LevelFile level;
	Level currentLevel;
	Vector2i spawn;
	Vector2i bossSpawn;

	Texture texTileset, texItems;
	Sprite  items[8];
	Sprite  tileset[8];
	Timer   timer;
	#pragma endregion 

	#pragma region Textures and sprites
	Texture texPlayer, texBackground, texBoss;
	Sprite  sprPlayer, sprBackground;
	RectangleShape     shapeBackground;
	#pragma endregion

	#pragma region Box2D
	b2Vec2  gravity;
	b2World *world;
	#pragma endregion

	#pragma region FPS counter
	Clock clockFps;
	int fps, frames;
	#pragma endregion

	#pragma region Level transition
	RectangleShape rectTrans;
	int transPos;
	float transSpeed;
	bool goToLevel;		// For when leaving the level
	#pragma endregion

	#pragma region Pause screen
	bool paused;
	float pauseScale;
	#pragma endregion 

	Shader shader;
	float speedMulti;
	vector<ForegroundTile> foregroundTiles;
	Color colFull, colMedium, colLow;
	Input input;

public:
	SceneLevel(Game *game)
	{
		#pragma region Vars
		this->window = game->window->getWindow();
		this->view   = game->window->getView();
		this->game   = game;

		fps = 0;
		speedMulti = 1.f;
		#pragma endregion

		#pragma region Load fonts and text
		if (!Util::loadFont(&fontDebug, "debug.ttf"))
			Engine::error("Failed to load debug font");

		if (!Util::loadFont(&fontPaused, "menu.ttf"))
			Engine::error("Failed to load menu font");

		Util::createText(
			&textInfo,
			"",
			&fontDebug,
			18,
			Text::Regular,
			Color(0, 0, 0),
			Vector2f(16, 16)
		);

		Util::createText(
			&textPaused,
			"PAUSED",
			&fontPaused,
			32,
			Text::Regular,
			Color(250, 250, 250),
			Vector2f(0, 0)
		);
		textPaused.setOrigin(textPaused.getLocalBounds().width / 2, textPaused.getLocalBounds().height);

		Util::createText(
			&textPausedInfo,
			"ESC: Unpause\nR: Restart level \nM: Back to menu",
			&fontDebug,
			18,
			Text::Regular,
			Color(245, 245, 245),
			Vector2f(0, 0)
		);
		textPausedInfo.setOrigin(textPausedInfo.getLocalBounds().width / 2, 0);
		if (input.isUsingController())
			textPausedInfo.setString("Start / ESC: Unpause\nY / R: Restart level \nSelect / M: Back to menu");

		Util::createText(
			&textTitle,
			"null",
			&fontPaused,
			26,
			Text::Regular,
			Color(250, 250, 250),
			Vector2f(0, 0)
		);
		textTitle.setOutlineColor(Color(5, 5, 5));
		textTitle.setOutlineThickness(3);
		#pragma endregion

		#pragma region Player tileset
		game->fileStream->open("tileset/player.tga");
		if (!texPlayer.loadFromStream(*game->fileStream))
			Engine::error("Failed to load player tileset");
		#pragma endregion 

		#pragma region Item tileset
		// Load texture from stream
		game->fileStream->open("tileset/items.tga");
		if (!texItems.loadFromStream(*game->fileStream))
			Engine::error("Failed to load items tileset");
		// Set textures
		for (auto i = 0; i < 8; i++)
		{
			items[i].setTexture(texItems);
			items[i].setTextureRect(IntRect(64 * i, 0, 64, 64));
		}
		#pragma endregion

		#pragma region Background
		game->fileStream->open("image/background.tga");
		if (!texBackground.loadFromStream(*game->fileStream))
			Engine::error("Failed to load background image");
		sprBackground.setTexture(texBackground);
		sprBackground.setScale(1.1, 1.1);

		shapeBackground.setSize(Vector2f(1280, 720));
		#pragma endregion 

		#pragma region Box2D
		gravity = b2Vec2(0, 10);
		world   = new b2World(gravity);
		world->SetContactListener(new ContactListener(&player, &game, reinterpret_cast<SceneLevel**>(&game->scenes.level)));
		#pragma endregion

		#pragma region Set level from save
		setLevel(game->save->level);
		textTitle.setString(currentLevel.name);
		#pragma endregion 

		#pragma region Player
		player = new Player(Vector2f(spawn.x, spawn.y), world, &texPlayer);
		/*
		shapePlayerCollision.setOutlineThickness(2);
		shapePlayerCollision.setOutlineColor(Color::Red);
		shapePlayerCollision.setFillColor(Color(0, 0, 0, 0));
		shapePlayerCollision.setSize(Vector2f(56, 56));
		*/
		#pragma endregion

		#pragma region Boss stuff
		if (currentLevel.type == LevelType::boss)
		{
			game->fileStream->open("tileset/enemy.tga");
			if (!texBoss.loadFromStream(*game->fileStream))
				Engine::error("Failed to load enemy tileset");

			boss = new Boss(Vector2f(bossSpawn.x, bossSpawn.y), world, &texBoss);

			// Not the best
			if (currentLevel.file.compare("2-B.clf") == 0)
				boss->type = Boss::classic;

			// Helth bar
			// Red:		244, 67,  54
			// Yellow:	255, 235, 59
			// Green:	76,  175, 80
			bossHealth.setFillColor(Color(76, 175, 80));	// We don't really need to set it here
			bossHealth.setSize(Vector2f(640, 6));			// Not this either
			bossHealth.setOutlineColor(Color::Black);
			bossHealth.setOutlineThickness(2);

			colFull   = Color(76,  175, 80);
			colMedium = Color(255, 235, 59);
			colLow    = Color(244, 67,  54);
		}
		#pragma endregion

		#pragma region Level transition
		rectTrans.setSize(Vector2f(1280, 720));
		rectTrans.setFillColor(Color::Black);
		rectTrans.setPosition(0, 0);
		transPos = 0;
		transSpeed = 44.25;

		goToLevel = false;
		#pragma endregion

		paused     = false;
		pauseScale = 1.005f;

		game->fileStream->open("shader/grayscale.frag");
		shader.loadFromStream(*game->fileStream, Shader::Fragment);
	}
	~SceneLevel()
	{
		// Clear all items
		cItems.clear();
		// Reset presents
		game->hud->setPresents(0);
	}
	void onEvent(const Event ev) override
	{
		if (ev.type == Event::KeyPressed)
		{
			// Keyboard presses
			if (ev.key.code == Keyboard::Escape)
				pause();
			else if (ev.key.code == Keyboard::R)
				reloadLevel();
			else if (ev.key.code == Keyboard::M)
			{
				view->reset(FloatRect(0, 0, 1280, 720));
				game->currentScene = game->scenes.menu;
				game->audio->playMusic("menu.xm");
				resetLevel();
			}
			else if (ev.key.code == Keyboard::P)
			{
				const auto filename = fmt::format("{}.png", to_string(system_clock::to_time_t(system_clock::now())));
				Texture texture;
				texture.create(1280, 720);
				texture.update(*window);
				auto image = texture.copyToImage();
				image.saveToFile(Engine::getWritePath(filename));
				Engine::log(fmt::format("Screenshot saved to {}", filename));
			}
		}
		else if (ev.type == Event::JoystickButtonPressed)
		{
			// Controller presses
			if (ev.joystickButton.button == input.getJoystickButton(Input::pause))
				pause();
			else if (ev.joystickButton.button == input.getJoystickButton(Input::restart))
				reloadLevel();
			else if (ev.joystickButton.button == input.getJoystickButton(Input::back))
			{
				view->reset(FloatRect(0, 0, 1280, 720));
				game->currentScene = game->scenes.menu;
				game->audio->playMusic("menu.xm");
				resetLevel();
			}
		}
		else if (ev.type == Event::LostFocus && !paused)
			pause();
	}
	void render(const Time frameTime) override
	{
		if (!paused)
			timer.tick(frameTime.asMicroseconds());

		#pragma region See if we still want to jump (Tweak or remove)
		/*
		if (player->getVelocity().y < 0 && !Keyboard::isKeyPressed(Keyboard::Up))
			player->setVelocity(player->getVelocity().x, player->getVelocity().y * 0.995);
		*/
		#pragma endregion 

		#pragma region Update debug text
		const auto viewRoot = Vector2f(view->getCenter().x - view->getSize().x / 2, view->getCenter().y - view->getSize().y / 2);
		if (game->devMode)
		{
			textInfo.setPosition(viewRoot.x + 16, viewRoot.y + 16);
			textInfo.setString(fmt::format("FPS: {}\nState: {}\nGrounded: {}\nPos: {}\nView: {:.0f} {:.0f}\nSpeed: {}\nTimer: {:.1f} \nDarkness: {:.1f}%\nDirection: {}\nSpeed: {:.1f}x\nTouching roof: {}", fps, player->getNameOfState(), player->getGroundedString(), player->getPositionString(), view->getCenter().x, view->getCenter().y, player->getSpeedString(), timer.getTimeSeconds(), (timer.getTimeSeconds() * 0.425) / 255 * 100, player->runningRight() ? "right" : "left", speedMulti, player->getTouchingRoofString()));
		}
		#pragma endregion

		#pragma region Check for level render error
		if (currentLevel.file == "")
			Engine::error("No level set to render");
		#pragma endregion

		#pragma region Move player
		if (player->stateCurrent != Player::hurting && player->stateCurrent != Player::dead)
		{
			if (input.isPressed(Input::left) && player->getVelocity().x >= -3.2f * speedMulti && !paused)
				player->move(-0.28f * speedMulti);
			if (input.isPressed(Input::right) && player->getVelocity().x <= 3.2f * speedMulti && !paused)
				player->move(0.28f * speedMulti);
			if (input.isPressed(Input::down) && player->getVelocity().x == 0 && player->getVelocity().y == 0 )
				player->duck();

			if (input.isPressed(Input::jump) && player->jump())
					game->audio->playSound(Audio::jump);
		}
		#pragma endregion

		// Box2D
		// 8,3 or 6,2?
		if (!paused)
			world->Step(1 / 60.f, 8, 3);

		#pragma region Update player and view position
		if (!paused)
			player->update(frameTime);
		//sprPlayer.setPosition(player->body->GetPosition().x * PixelsPerMeter, player->body->GetPosition().y * PixelsPerMeter - 36);
		//view->setCenter(player->body->GetPosition().x * PixelsPerMeter, player->body->GetPosition().y * PixelsPerMeter);
		if (player->stateCurrent != Player::hurting && player->stateCurrent != Player::dead)
			view->setCenter(int(player->getPosition().x + 0.5f), int(player->getPosition().y + 0.5f));

		// If player is above 1600, we are below the map
		// TODO: This is not ideal, but it works
		if (player->getPosition().y > 1700 && player->getPosition().y < 1800)
			reloadLevel();
		#pragma endregion

		#pragma region Check if view is outside of level
		// Here we just sorta assume the level is 4800x1600 (75x25 tiles)
		if (view->getCenter().x < 640)
			view->setCenter(640, view->getCenter().y);
		if (view->getCenter().y > 1240)
			view->setCenter(view->getCenter().x, 1240);
		if (view->getCenter().x > 4160)
			view->setCenter(4160, view->getCenter().y);
		if (view->getCenter().y < 360)
			view->setCenter(view->getCenter().x, 360);
		#pragma endregion

		#pragma region Clear and draw background

		// After 600s, background should be 0
		// (x0.425)

		if (currentLevel.type == LevelType::stage)
		{
			window->clear(Color(51, 153, 218));

			// These needs to be adjusted
			sprBackground.setPosition((viewRoot.x) - (view->getCenter().x * 0.03), viewRoot.y - 72);
			window->draw(sprBackground, getShader());
		}
		else
		{
			// Have to be boss
			if (boss->type == Boss::follow)
				window->clear(Color(134, 149, 149));
			else
				window->clear(Color(67, 75, 75));
		}

		// Overlay to not make it too bright
		shapeBackground.setPosition(viewRoot);
		// Black (*0.95 to we can still see)
		if (timer.getTimeSeconds() < 600)
			shapeBackground.setFillColor(Color(0, 0, 0, (timer.getTimeSeconds() * 0.425) * 0.95));
		// Sunset color
		//shapeBackground.setFillColor(Color(253, 94, 83, timer.getElapsedTime().asSeconds() * 4.25));
		//window->draw(shapeBackground);
		#pragma endregion

		#pragma region Draw level
		for (auto x = 0; x < level.mapSize[0]; x++)
			for (auto y = 0; y < level.mapSize[1]; y++)
			{
				const auto tile = level.map[x][y];
				if (tile >= 0)
				{
					if (tile < 50)
					{
						// It's a block
						tileset[tile].setPosition(x * level.tileSize, y * level.tileSize);
						window->draw(tileset[tile], getShader());
					}
					else if (tile == 51)
					{
						// The exit
						if (game->hud->allObjectivesCollected())
						{
							items[1].setPosition(x * level.tileSize, y * level.tileSize);
							window->draw(items[1], getShader());
						}
					}
					else if (tile != 52 && tile != 53 && tile != 57 && tile != 54)
					{
						// Drawing 57 (boss) casues a line to be rendered
						// We also draw water (54) / foreground objects later

						// It's an item
						// (Ignore collectables and draw them later)
						const auto item = tile - 50;
						items[item].setPosition(x * level.tileSize, y * level.tileSize);
						window->draw(items[item], getShader());
					}
				}
			}
		// Draw coins and stuff
		for (auto const& citem : cItems)
		{
			// Only draw if not collected
			if (!citem.collected)
			{
				items[citem.tileID].setPosition(citem.getPosition());
				window->draw(items[citem.tileID], getShader());
			}
		}
		#pragma endregion

		#pragma region Update transition
		// This together with '<= 0' fixes weird line
		if (transPos == 24)
			transPos = 0;
		rectTrans.setPosition(viewRoot.x + transPos, viewRoot.y);
		if (goToLevel)
		{
			// Exit
			if (transPos <= 0)
			{
				transPos += transSpeed;
				transSpeed += 0.75;
			}
			else if (currentLevel.type == LevelType::boss && boss->health <= 0)
			{
				// We finished the last boss, go to SceneEndGame instead

				goToLevel = false;

				view->reset(FloatRect(0, 0, 1280, 720));
				game->currentScene = game->scenes.endgame;
				game->audio->playMusic("credits.it");
				resetLevel();
			}
			else
			{
				// Pretty sure this isn't needed
				goToLevel = false;

				// Clear memory and reload scene
				resetLevel();

				// Load new music
				game->audio->playMusic(currentLevel.music);
				// Also restore just to be sure
				game->audio->lowerVolume(1.0f);

				// Seems to fix the white flash (but hud flashes)
				window->clear();
			}
		}
		else if (transPos < 1280)
		{
			// Enter
			transPos += transSpeed;
			transSpeed -= 0.75;
		}
		#pragma endregion 

		#pragma region Draw everything
		window->draw(*player->getSprite(frameTime), getShader());
		for (auto const& tile : foregroundTiles)
		{
			// Only water for now
			items[4].setPosition(tile.getPosition());
			window->draw(items[4], getShader());
		}
		window->draw(shapeBackground);
		if (currentLevel.type == LevelType::boss)
		{
			#pragma region Update boss position
			boss->update();

			float moveX;
			float moveY;

			const auto diffX = player->getBodyPosition().x - boss->getBodyPosition().x - 30;
			const auto diffY = player->getBodyPosition().y - boss->getBodyPosition().y - 48;

			if (diffX < 2 && diffX > -2)
				moveX = 0;
			else if (player->getPosition().x < boss->getPosition().x)
				moveX = -1.15f;
			else
				moveX = 1.15f;

			if (diffY < 2 && diffY > -2)
				moveY = 0;
			else if (player->getPosition().y < boss->getPosition().y - 48)
				moveY = -1.15f;
			else
				moveY = 1.15f;

			if (boss->type == Boss::classic)
			{
				// Move much slower up and down
				moveY = moveY * 0.02f;

				// Move opposite direction when we jump
				const auto dist = player->getBodyPosition().x - boss->getBodyPosition().x;
				if (player->stateCurrent == Player::jumping)
				{
					//cout << "PLAYER_DISTANCE_" << player->getBodyPosition().x - boss->getBodyPosition().x << endl;

					moveX = moveX * -0.5f;
					boss->setAngry(false);
				}
				else
					boss->setAngry(true);

				/*
				* 8hp	1x		/	1x
				* 6hp	1.25x	/	1.5x	+0.5
				* 4hp	1.75x	/	2.5x	+1
				* 2hp	2x		/	4x		+1.5
				*/

				float bossMulti;

				if (boss->health <= 2)
				{ bossMulti = 4.f; bossHealth.setFillColor(colLow); }
				else if (boss->health <= 4)
				{ bossMulti = 2.5f; bossHealth.setFillColor(colMedium); }
				else if (boss->health <= 6)
					bossMulti = 1.5f;
				else
				{ bossHealth.setFillColor(colFull); bossMulti = 1.f; }

				moveX = moveX * bossMulti;
			}

			if (player->stateCurrent != Player::dead)
				boss->move(moveX, moveY);
			else
				boss->move(moveX * 0.05f, moveY * 0.05f);
			#pragma endregion

			if (boss->type == Boss::classic && !paused && boss->health > 0)
			{
				// If classic, draw health bar

				// Boss has 8 health
				// 8hp = 640px wide
				// 80px per hp

				if (bossHealth.getSize().x > 80 * boss->health)
					bossHealth.setSize(Vector2f(bossHealth.getSize().x - 8, 6));

				bossHealth.setOrigin(bossHealth.getSize().x / 2, 0);	// Assume it's 4+2=6 high

				bossHealth.setPosition(viewRoot.x + 640, viewRoot.y + 32);
				window->draw(bossHealth, getShader());
			}

			if (boss->health > 0)
				window->draw(*boss->getSprite(), getShader());
		}
		window->draw(textInfo);
		if (!paused)
			game->hud->draw(viewRoot);

		if (paused)
		{
			// Enlarge and shrink the text, because why not
			textPaused.setScale(textPaused.getScale().x * pauseScale, textPaused.getScale().y * pauseScale);
			if (textPaused.getScale().x > 1.1)
				pauseScale = 0.995f;
			else if (textPaused.getScale().x < 0.9)
				pauseScale = 1.005f;

			// Position can also be set from pause()
			textPaused.setPosition(viewRoot.x + 640, viewRoot.y + 352);
			textPausedInfo.setPosition(viewRoot.x + 640, viewRoot.y + 368);

			window->draw(textPaused);
			window->draw(textPausedInfo);
		}

		if (timer.getTimeSeconds() <= 3 && !paused)
		{
			textTitle.setOrigin(textTitle.getLocalBounds().width / 2, textTitle.getLocalBounds().height / 2);

			if (timer.getTimeSeconds() >= 2)
				textTitle.setScale(textTitle.getScale().x * 0.9, textTitle.getScale().y * 0.9);

			textTitle.setPosition(viewRoot.x + 640, viewRoot.y + 360);
			window->draw(textTitle);
		}

		window->draw(rectTrans);
		#pragma endregion 

		#pragma region Calculate FPS
		// Less accurate, but easier to look at
		if (clockFps.getElapsedTime().asSeconds() >= 1)
		{
			fps = frames;
			frames = 0;
			clockFps.restart();
		}
		else
			frames++;
		#pragma endregion
	}

	void setLevel(const int index)
	{
		#pragma region CLF
		// Check if level exists
		if (game->levels[index] == nullptr)
			Engine::error(fmt::format("Level {} not found", index));

		currentLevel = *game->levels[index];
		Engine::log(fmt::format("Loading {} ({})...", currentLevel.file, index));

		if (!Level::loadLevel(&level, currentLevel.file))
			Engine::error("Failed to load level " + currentLevel.file);
		#pragma endregion

		setTileset(level.mainTileset);

		#pragma region Physics engine
		b2BodyDef      bodyDef;
		b2PolygonShape shape;
		b2FixtureDef   fixtureDef;

		auto numObjectives = 0;

		for (auto x = 0; x < level.mapSize[0]; x++)
			for (auto y = 0; y < level.mapSize[1]; y++)
			{
				const auto tile = level.map[x][y];
				if (tile >= 0)
				{
					// TODO: These really need to be functions instead
					if (tile < 4)
					{
						createBox(b2_staticBody, x, y, 32, 32, BitGround, false);

						// Full (64x64)
						/*
						bodyDef.type = b2_staticBody;
						bodyDef.position.Set(x * 64 / PixelsPerMeter, y * 64 / PixelsPerMeter);
						auto *body = world->CreateBody(&bodyDef);
						shape.SetAsBox(32 / PixelsPerMeter, 32 / PixelsPerMeter);
						fixtureDef.shape = &shape;
						fixtureDef.filter.categoryBits = BitGround;
						fixtureDef.isSensor = false;
						fixtureDef.userData = nullptr;
						//fixtureDef.density = 0; // ?
						body->CreateFixture(&fixtureDef);
						*/
					}
					else if (tile < 50)
					{
						// Half (64x32)
						bodyDef.type = b2_staticBody;
						bodyDef.position.Set(x * 64 / PixelsPerMeter, (y * 64 - 16) / PixelsPerMeter);
						auto *body = world->CreateBody(&bodyDef);
						shape.SetAsBox(32 / PixelsPerMeter, 16 / PixelsPerMeter);
						fixtureDef.shape = &shape;
						fixtureDef.filter.categoryBits = BitPlatform;
						fixtureDef.isSensor = false;
						fixtureDef.userData = nullptr;
						//fixtureDef.density = 0; // ?
						body->CreateFixture(&fixtureDef);
					}
					else if (tile == 50)
					{
						// Spawn
						spawn.x = x * 64;
						spawn.y = y * 64;
					}
					else if (tile == 51)
					{
						// Exit
						createBox(b2_staticBody, x, y, 64, 64, BitExit, false);
					}
					else if (tile == 56 || tile == 55)
					{
						// Spike (Or lava)
						bodyDef.type = b2_staticBody;
						bodyDef.position.Set(x * 64 / PixelsPerMeter, (y * 64 + 16) / PixelsPerMeter);
						auto *body = world->CreateBody(&bodyDef);
						shape.SetAsBox(32 / PixelsPerMeter, 16 / PixelsPerMeter);
						fixtureDef.shape = &shape;
						fixtureDef.isSensor = false; // Mess with this
						//fixtureDef.userData = reinterpret_cast<short*>(dataSpikes);
						fixtureDef.filter.categoryBits = BitSpike;
						body->CreateFixture(&fixtureDef);
					}
					else if (tile == 57)
					{
						// Boss
						bossSpawn.x = x * 64;
						bossSpawn.y = y * 64;
					}
					else if (tile == 54)
					{
						// Water tile
						// Similar to normal box, but it's a sensor
						createBox(b2_staticBody, x, y, 32, 32, BitWater, true);
						foregroundTiles.push_back(ForegroundTile(x, y, 4));
					}
					else
					{
						// Count number of objectives / presents
						if (tile == 53)
							numObjectives++;

						// Some other item
						// This is very similar to full body
						bodyDef.type = b2_staticBody;
						bodyDef.position.Set(x * 64 / PixelsPerMeter, y * 64 / PixelsPerMeter);
						auto *body = world->CreateBody(&bodyDef);
						shape.SetAsBox(26 / PixelsPerMeter, 32 / PixelsPerMeter);
						fixtureDef.shape = &shape;
						fixtureDef.isSensor = true;
						// BitCoin is basically BitItem?
						fixtureDef.filter.categoryBits = BitCoin;

						// Set userdata
						// Are these even used for anything?
						/*
						switch (tile)
						{
							case 51: fixtureDef.userData = reinterpret_cast<short*>(dataExit);   break;
							case 52: fixtureDef.userData = reinterpret_cast<short*>(dataCoin);   break;
							case 53: fixtureDef.userData = reinterpret_cast<short*>(dataMain);   break;
							case 54: fixtureDef.userData = reinterpret_cast<short*>(dataBonus);  break;
							case 55: fixtureDef.userData = reinterpret_cast<short*>(dataWater);  break;
							case 56: fixtureDef.userData = reinterpret_cast<short*>(dataLava);   break;
							case 57: fixtureDef.userData = reinterpret_cast<short*>(dataSpikes); break; // Unused?
							default: break;
						}
						*/

						// Set exit bit
						if (tile == 51)
							fixtureDef.filter.categoryBits = BitExit;

						// See if we need to add it to cItems
						if (tile == 52)
						{
							cItems.push_back(CollectableItem(coin, x, y));
							fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(cItems.size());
						}
						else if (tile == 53)
						{
							cItems.push_back(CollectableItem(objective, x, y));
							fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(cItems.size());
						}

						body->CreateFixture(&fixtureDef);
					}

					/*
					if (tile > 5 && tile < 50)
					{
						// Half (64x32)
						bodyDef.type = b2_staticBody;
						bodyDef.position.Set(x * 64 / PixelsPerMeter, (y * 64 - 16) / PixelsPerMeter);
						auto *body = world->CreateBody(&bodyDef);
						shape.SetAsBox(32 / PixelsPerMeter, 16 / PixelsPerMeter);
						fixtureDef.shape = &shape;
						fixtureDef.filter.categoryBits = BitPlatform;
						//fixtureDef.density = 0; // ?
						body->CreateFixture(&fixtureDef);
					}
					else if (tile < 50)
					{
						// Full (64x64)
						bodyDef.type = b2_staticBody;
						bodyDef.position.Set(x * 64 / PixelsPerMeter, y * 64 / PixelsPerMeter);
						auto *body = world->CreateBody(&bodyDef);
						shape.SetAsBox(32 / PixelsPerMeter, 32 / PixelsPerMeter);
						fixtureDef.shape = &shape;
						fixtureDef.filter.categoryBits = BitGround;
						//fixtureDef.density = 0; // ?
						body->CreateFixture(&fixtureDef);
					}
					else
						Engine::log("Unknown tile");
					*/
				}
			}

		game->hud->setNumPresents(numObjectives);
		#pragma endregion

		#pragma region TMX (Unused)
		//map->load(currentLevel.file);
		/*
		auto& player = map->getLayers().at(1);
		Engine::log(fmt::format("Checking layer '{}' for player spawn...", player.name));
		for (auto& obj : player.objects)
		{
			int x = spawn.x = obj.getPosition().x;
			int y = spawn.y = obj.getPosition().y;

			shapeSpawn.setPosition(x, y);
			sprPlayer.setPosition(x, y);

			int spawnX = obj.getPosition().x / 64;
			int spawnY = obj.getPosition().y / 64;
			Engine::log(fmt::format("Spawn at {}x{} ({}x{})", obj.getPosition().x, obj.getPosition().y, spawnX, spawnY));
		}
		*/
		#pragma endregion
	}
	void setTileset(const string name)
	{
		if (name.find(".png") != string::npos)
			Engine::log("Warning: Deprecated tileset. Update to json2clf 1.3 or newer.");

		if (!game->fileStream->open(("tileset/" + name).c_str()))
			Engine::error("Failed to open stream for tileset " + name);
		if (!texTileset.loadFromStream(*game->fileStream))
			Engine::error("Failed to set texture from stream");

		for (auto i = 0; i < 8; i++)
		{
			tileset[i].setTexture(texTileset);
			tileset[i].setTextureRect(IntRect(64 * i, 0, 64, 64));
		}
	}
	void reloadLevel()
	{
		// Sets coin to what we have saved
		//game->hud->updateCoins(game->save->coins);

		// Starts transition and changes level later
		rectTrans.setPosition(-1280, 720);
		goToLevel = true;
		transPos = -1280;
		transSpeed = 0.f;

		// Testing
		// If we save before reloading level, this won't be an issue
		game->hud->loadFromSave();
	}
	void enter()
	{
		// Semi-temp function to restore everything and start music
		// This is mostly a fairly bad way to fix music and timer bugs
		timer.restart();
		game->audio->playMusic(currentLevel.music);
	}
	void pause()
	{
		// Toggle pause
		paused = !paused;

		// Temporarily lower the volume
		if (paused)
			game->audio->lowerVolume(0.5f);
		else
			game->audio->lowerVolume(1.0f);


		// Do more stuff later...
	}
	void resetLevel() const
	{
		// Clears some memory, but not all
		// Fixes proper restart position though, but respawns all items
		// This crashes sometimes? (game is null) Hopefully this (or &*game) fixes it
		// Spoiler: It doesn't (Though, sorta, since game is now something else rather than nullptr)
		// If it's null, it gives a write access violation (Can't be caught)

		// Since the issue is prob related to clearing the level before
		// creating the new one, maybe setting static or allocating
		// heap memory fixes the issue

		//Game* temp = new Game(*this->game);

		//auto tempGame = this->game;
		//delete game->scenes.level;
		//this->game->scenes.level = new SceneLevel(tempGame);

		// Moving it to a seperate class should fix the crash
		LevelSwitcher::reset(this->game);
	}
	Boss* getBoss() const
	{
		if (this->currentLevel.type == LevelType::boss)
			return this->boss;
		return nullptr;
	}

	float getElapsedTime() const
	{
		return timer.getTimeSeconds();
	}
	bool goingToLevel() const
	{ return goToLevel; }
	void setGoToLevel(const bool goToLevel)
	{
		this->goToLevel = goToLevel;
	}
	void setSpeedMulti(const float speed)
	{
		this->speedMulti = speed;
	}

	// Testing: Create body helper
	void createBox(const b2BodyType bodyType, const int x , const int y, const int width, const int height, const short categoryBits, const bool isSensor) const
	{
		b2BodyDef      bodyDef;
		b2PolygonShape shape;
		b2FixtureDef   fixtureDef;

		bodyDef.type = bodyType;
		bodyDef.position.Set(x * 64 / PixelsPerMeter, y * 64 / PixelsPerMeter);
		auto body = world->CreateBody(&bodyDef);
		
		shape.SetAsBox(width / PixelsPerMeter, height / PixelsPerMeter);

		fixtureDef.shape = &shape;
		fixtureDef.filter.categoryBits = categoryBits;
		fixtureDef.isSensor = isSensor;

		body->CreateFixture(&fixtureDef);
	}

	Shader* getShader()
	{
		if (paused)
			return &shader;
		return nullptr;
	}
};

inline void ContactListener::BeginContact(b2Contact* contact)
{
	auto *a = contact->GetFixtureA();
	auto *b = contact->GetFixtureB();
	const auto collisionDef = a->GetFilterData().categoryBits | b->GetFilterData().categoryBits;

	// We only really need to find the item
	b2Fixture* fixtureItem = nullptr;
	if (a->GetFilterData().categoryBits == BitCoin)
		fixtureItem = a;
	else if (b->GetFilterData().categoryBits == BitCoin)
		fixtureItem = b;

	// Just so it compiles
	auto             index     = -1;
	CollectableItem* collected = nullptr;
	Boss*            boss      = nullptr;

	switch (collisionDef)
	{
		case BitPlayer     | BitCoin:
		case BitPlayerHead | BitCoin:
			// Player collected coin or present
			index = reinterpret_cast<uintptr_t>(fixtureItem->GetUserData().pointer) - 1; // Don't assume it's b
			collected = &cItems.at(index);
			if (!collected->collected)
			{
				collected->collected = true;
				if (collected->type == coin)
				{
					(*hud)->updateCoins(1);
					(*audio)->playSound(Audio::coin);
				}
				else if (collected->type == objective)
				{
					(*hud)->updatePresents(1);
					(*audio)->playSound(Audio::present);
				}
			}
			break;
		case BitPlayer | BitBoss:
			boss = (*level)->getBoss();
			if (boss->type == Boss::classic)
			{
				if ((*player)->getVelocity().y > 0 && (*player)->getBodyPosition().y < boss->getBodyPosition().y)
				{
					boss->health--;
					// We normally jump -5.3
					(*player)->jump(-6.3f);

					// Velocity or time as seed?
					const auto rng = Util::getRandomNumber(900, 1200);

					// Move boss
					if ((*player)->runningRight())
						boss->move(-rng, 0);
					else
						boss->move(rng, 0);

					// Play sound
					(*audio)->playSound(Audio::boss);

					// See if boss died
					if (boss->health <= 0)
					{
						Engine::log(fmt::format("Final boss completed in {} seconds", (*level)->getElapsedTime()));
						(*save)->totalTime += (*level)->getElapsedTime();

						boss->die();
						(*level)->reloadLevel();
					}

					break;
				}
			}
			// We got hit by the boss
			// Similar to spikes, but we don't get fly away as much
			(*player)->die(4, 4);
			(*audio)->playSound(Audio::fall);
			(*save)->totalDeaths++;
			(*save)->totalTime += (*level)->getElapsedTime();
			break;

		case BitPlayer | BitSpike:
			// Player hit some spikes, die
			(*player)->die();
			(*audio)->playSound(Audio::fall);
			(*save)->totalDeaths++;
			(*save)->totalTime += (*level)->getElapsedTime();
			break;
		case BitPlayer | BitExit:
			if ((*hud)->allObjectivesCollected() && !(*level)->goingToLevel())
			{
				// For speedrunning I guess
				Engine::log(fmt::format("Level completed in {} seconds", (*level)->getElapsedTime()));
				(*save)->totalTime += (*level)->getElapsedTime();

				(*save)->coins = (*hud)->getCoins();
				(*save)->level++;
				(*level)->reloadLevel();

				(*audio)->playSound(Audio::complete);

				(*save)->save("save");
			}
			break;
		case BitPlayer | BitWater:
			(*level)->setSpeedMulti(0.5f);
			break;
		case BitPlayerHead | BitGround:
			(*player)->setTouchingRoof(true);
			break;
		default: break;
	}
}
inline void ContactListener::EndContact(b2Contact* contact)
{
	auto *a = contact->GetFixtureA();
	auto *b = contact->GetFixtureB();
	const auto collisionDef = a->GetFilterData().categoryBits | b->GetFilterData().categoryBits;

	switch (collisionDef)
	{
		case BitPlayer | BitWater:
			(*level)->setSpeedMulti(1.f);
			break;
		case BitPlayerHead | BitGround:
			(*player)->setTouchingRoof(false);
			break;
		default: break;
	}
}
inline void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	auto *a = contact->GetFixtureA();
	auto *b = contact->GetFixtureB();
	const auto collisionDef = a->GetFilterData().categoryBits | b->GetFilterData().categoryBits;

	// Check for one-way platforms
	if (collisionDef == (BitPlayer | BitPlatform))
	{
		if ((*player)->body->GetLinearVelocity().y < 0)
			contact->SetEnabled(false);
		else
			contact->SetEnabled(true);
	}
}
inline void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	// Unused, for now at least
}

inline void LevelSwitcher::reset(Game* game)
{
	// Just to be sure
	const auto gamePtr = game;

	delete gamePtr->scenes.level;
	gamePtr->scenes.level = new SceneLevel(gamePtr);
}