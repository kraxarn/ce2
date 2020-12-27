#pragma once

#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>

class SceneHUD;
using namespace std;

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_physfs.h>

#include <box2d/box2d.h>

#include <physfs.h>

// Our other engine files
#include "FileStream.hpp"
#include "AnimatedSprite.hpp"
#include "CrowLevel.hpp"
#include "CrowAudio.hpp"
#include "WindowIcon.h"

// For logging useless debug info
#include <fmt/format.h>
#ifdef CE_DEBUG
#include <SFML/OpenGL.hpp>
#endif

namespace ce
{
	#pragma region Global variables
	// Detect OS
	#if defined SFML_SYSTEM_WINDOWS
		const string EngineOS = "windows";
		#define CE_WINDOWS
	#elif defined SFML_SYSTEM_LINUX
		const string EngineOS = "linux";
		#define CE_LINUX
	#elif defined SFML_SYSTEM_MACOS
		const string EngineOS = "macos";
		#define CE_MACOS
	#else
		#error Error: Unsupported OS
	#endif

	// Version vars
	//const string EngineVersion    = "2.0.0";
	//const string FrameworkVersion = "1.0.0";
	//const string GameVersion      = "1.0.0";

	// Framework and game name
	//const string FrameworkName = "CrowPlatformer";
	const string GameName      = "KraXmas 2";
	const string ProjectName   = "KraXmas2";	// Old game only used .ini files anyway

	// Since we can't create variables in Engine
	//ALLEGRO_TEXTLOG *textLog;

	// Box2D Constants
	const float PixelsPerMeter = 100;

	// Box2D filters
	const short BitNothing    = 0;
	const short BitDestroyed  = 1;
	const short BitGround     = 2;
	const short BitPlatform   = 4;
	const short BitPlayer     = 8;
	const short BitCoin       = 16;
	const short BitSpike      = 32;
	const short BitExit       = 64;
	const short BitBoss       = 128;
	const short BitWater      = 256;
	const short BitPlayerHead = 512;
	#pragma endregion

	#pragma region --- Core ---

	class Config;

	class Engine
	{
	public:
		/// @brief Loads the engine
		/// @param devMode Enable developer mode
		Engine(bool devMode);

		/// @brief Logs to console or file
		/// @param message Message to log (without new line)
		static void log(string message);

		/// @brief Shows error messages and closes the game
		/// @param message Message to show
		static void error(string message);

		/// @brief Shows a warning message, but doesn't close the game
		/// @param message Message to show
		static void warning(string message);

		/// @brief Gets the path to a writable directory
		/// @param file Optional file to concatenate
		static string getWritePath(string file);

		/// @brief Checks if file exists in write directory
		/// @param fileName File to check
		static bool fileExists(string fileName);
	};

	#pragma endregion 

	#pragma region --- Window manager ---

	class Window
	{
		RenderWindow window;
		View view;
	public:

		/// @brief Creates window (1280x720 60 fps) and view (1280x720)
		/// @param videoMode Window video mode
		/// @param virtualRes Virtual resolution
		/// @param fps Framerate limit
		Window(const VideoMode videoMode, const Vector2f virtualRes, const int fps)
		{
			window.create(videoMode, GameName, Style::None);
			window.setFramerateLimit(fps);
			//window.setVerticalSyncEnabled(true);
			view.reset(FloatRect(0, 0, virtualRes.x, virtualRes.y));
			window.setIcon(icon.width, icon.height, icon.pixel_data);

			/*
			DEVMODE devMode;
			if (EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &devMode))
				Engine::log(fmt::format("Monitor is {} hz", devMode.dmDisplayFrequency));
			*/
		}

		/// @return Returns current window
		RenderWindow* getWindow()
		{
			return &window;
		}

		/// @return Returns current view for window
		View* getView()
		{
			return &view;
		}

		/// @brief Centers the current window
		void centerWindow()
		{
			const auto desktopMode = VideoMode::getDesktopMode();
			const auto x = desktopMode.width / 2  - this->window.getSize().x / 2;
			const auto y = desktopMode.height / 2 - this->window.getSize().y / 2;
			this->window.setPosition(Vector2i(x, y));
		}

		/// @brief Set size and center current window
		void setSize(const float scale)
		{
			this->window.setSize(Vector2u(1280 * scale, 720 * scale));
			centerWindow();
		}
	};

	#pragma endregion

	#pragma region --- Utilities ---

	class Util
	{
	public:
		/// @brief Wrapper around various required stuff to create text
		static void createText(Text* text, const String string, const Font* font, const int size, const int style, const Color color, const Vector2f position)
		{
			text->setString(string);
			text->setFont(*font);
			text->setCharacterSize(size);
			text->setStyle(style);
			text->setFillColor(color);
			text->setPosition(position);
		}

		/// @brief Get name of Box2D filter
		static string getNameOfFilter(const short filter)
		{
			switch (filter)
			{
				case BitNothing:   return "nothing";
				case BitDestroyed: return "destroyed";
				case BitGround:    return "ground";
				case BitPlatform:  return "platform";
				case BitPlayer:    return "player";
				case BitCoin:      return "coin";
				default:           return "unknown";
			}
		}

		/// @brief Load font into memory retrun it to the font
		static bool loadFont(Font* font, const string name)
		{
			// We can't remove the old memory since that will cause the font to fail
			// For now we just add an warning that will probably be removed later anyway
			// The best fix would probably be to load all fonts in the Game class and reuse

			auto* file = PHYSFS_openRead(("font/" + name).c_str());
			const auto size = PHYSFS_fileLength(file);
			auto* buffer = new char[size];
			PHYSFS_readBytes(file, buffer, size);
			//Engine::log(fmt::format("Warning: Lost {} kilobytes of memory", (sizeof(buffer) * size) / 1000));
			if (!font->loadFromMemory(buffer, size))
				return false;
			return true;
		}

		/// @brief Generate a random number
		static int getRandomNumber(const int min, const int max, const int seed = random_device()())
		{
		    /*
			default_random_engine random;
			random.seed(seed);
			const uniform_int_distribution<int> dist(min, max);
			return dist(random);
		     */

		    // Maybe?
		    random_device random;
		    mt19937 rng(random());
		    uniform_int_distribution<mt19937::result_type> dist((ulong) min, (ulong) max);
		    return (int) dist(rng);
		}

		/// @brief Formats ms to mm::ss.ms
		static string formatTime(int ms)
		{
			// Vars
			auto min = 0, sec = 0;

			// Minutes
			while (ms >= 60000)
			{
				ms -= 60000;
				min++;
			}

			// Seconds
			while (ms >= 1000)
			{
				ms -= 1000;
				sec++;
			}

			// Return
			return fmt::format("{}:{}.{}", min, sec, ms);
		}
	};

	#pragma endregion 

	#pragma region --- Actors ---

	class Player
	{
		Texture tileset;

		//float stateTime;
		bool isRunningRight, isDead, isTouchingRoof;

		Animation      animStand, animRun, animJump, animFall, animHurt, animDead, animDuck;
		Animation      *animCurrent;
		AnimatedSprite animation;

	public:
		enum State { standing, running, jumping, falling, hurting, dead, ducking };
		State stateCurrent, statePrevious;

		// Most of these should be private and modified with .move() for example
		Sprite   sprite;
		Vector2f spawn;
		Vector2f position;
		b2Body   *body;

		Player(const Vector2f spawn, b2World *world, Texture* tileset)
		{
			// Public vars
			this->spawn = spawn;
			//this->grounded      = false;
			this->stateCurrent = standing;
			this->statePrevious = standing;

			// Private vars
			//this->stateTime = 0;
			this->isRunningRight = true;
			this->isDead         = false;
			this->tileset        = *tileset;
			this->isTouchingRoof = false;

			// Load stuff
			loadAnimations();

			// Box2D
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(spawn.x / PixelsPerMeter, spawn.y / PixelsPerMeter);
			bodyDef.fixedRotation = true;

			body = world->CreateBody(&bodyDef);

			// Edge just for collision
			// (This prob needs a lot of fixing, we need 62 though to pick up coins)
			createEdgeShape(
				b2Vec2(-58 / PixelsPerMeter, -62 / PixelsPerMeter),
				b2Vec2(-6 / PixelsPerMeter,  -62 / PixelsPerMeter),
				BitPlayer,
				BitCoin | BitExit | BitBoss | BitGround,	// Colliding with ground to make sure we aren't roof jumping
				0.f,	// Doesn't matter since it's a sensor
				true	// So we don't lose momentum by colliding with stuff
			);

			// Edge like the home shape, but slightly higher up so we glide against walls
			createEdgeShape(
				b2Vec2(-59 / PixelsPerMeter, -1 / PixelsPerMeter),
				b2Vec2(-5 / PixelsPerMeter,  -1 / PixelsPerMeter),
				BitPlayer,
				BitGround | BitPlatform,	// No need for anything else
				0.f,						// Maybe adjust this so we at least have a little friction
				false
			);

			// Edge, probably the best, at least for the feet
			createEdgeShape(
				b2Vec2(-58 / PixelsPerMeter, 0),
				b2Vec2(-6  / PixelsPerMeter, 0),
				BitPlayer,
				BitGround | BitPlatform | BitCoin | BitSpike | BitBoss | BitWater,
				0.4f,
				false
			);

			// Head, just to check if we're 'roof jumping'
			createEdgeShape(
				b2Vec2(-57 / PixelsPerMeter, -2 / PixelsPerMeter),
				b2Vec2(-7 / PixelsPerMeter,  -2 / PixelsPerMeter),
				BitPlayerHead,
				BitGround,	// We only need to collide with ground
				0.f,		// Doesn't matter
				true
			);
		}

		void update(const Time frameTime)
		{
			// Update state
			//this->stateCurrent = getState();

			// Update animation
			switch (stateCurrent)
			{
				case running: animCurrent = &animRun;   break;
				case jumping: animCurrent = &animJump;  break;
				case hurting:
				case dead:    animCurrent = &animHurt;  break;
				case ducking: animCurrent = &animDuck;  break;
				default:      animCurrent = &animStand; break;
			}

			// Update position variable
			// Cast to int to fix lines
			//position = Vector2f(static_cast<int>(body->GetPosition().x * PixelsPerMeter + 1), static_cast<int>(body->GetPosition().y * PixelsPerMeter - 62));
			position = Vector2f(body->GetPosition().x * PixelsPerMeter + 1, body->GetPosition().y * PixelsPerMeter - 63);

			// Update animated sprite
			animation.play(*animCurrent);
			animation.update(frameTime);
			animation.setPosition(position);
		}
		State getState() const
		{
			// Check if we are dead
			if (isDead)
				return dead;
			// Then if we're jumping or is in the air because we got hurt
			if (body->GetLinearVelocity().y < 0 || (body->GetLinearVelocity().y > 0 && statePrevious == jumping))
			{
				if (stateCurrent == hurting)
					return hurting;
				return jumping;
			}
			// If we are on our way down, we're falling
			if (body->GetLinearVelocity().y > 0)
				return falling;
			// If x isn't 0, we're running
			if (body->GetLinearVelocity().x != 0)
				return running;
			// Default
			return standing;
		}

		AnimatedSprite* getSprite(Time frameTime)
		{
			// Update state
			this->stateCurrent = getState();
			// See if we need to flip
			if ((body->GetLinearVelocity().x < 0 || !isRunningRight) && !isFlipX())
			{
				animation.setScale(-1, 1);
				isRunningRight = false;
			}
			else if ((body->GetLinearVelocity().x > 0 || isRunningRight) && isFlipX())
			{
				animation.setScale(1, 1);
				isRunningRight = true;
			}

			// If we switched state, reset stateTime
			//stateTime = stateCurrent == statePrevious ? stateTime + frameTime.asMilliseconds() : 0;
			// Also stop current animation (Really needed?)
			if (stateCurrent != statePrevious)
				animation.stop();
			// Update previous state
			statePrevious = stateCurrent;

			// Return current animation
			return &animation;
		}
		void move(const float x) const
		{
			body->ApplyLinearImpulse(b2Vec2(x, 0), body->GetWorldCenter(), true);
		}
		/// @return If we jumped successfully
		bool jump() const
		{
			// If we're dead or already jumping, we don't want to jump
			if (stateCurrent == hurting || stateCurrent == dead || stateCurrent == jumping)
				return false;

			// TESTING: If we aren't grounded, don't jump
			if (!isGrounded())
				return false;

			body->ApplyLinearImpulse(b2Vec2(0, -5.3), body->GetWorldCenter(), true);
			return true;
		}
		void jump(const float y) const
		{
			body->ApplyLinearImpulse(b2Vec2(0, y), body->GetWorldCenter(), true);
		}
		void duck()
		{
			stateCurrent = ducking;
		}
		void die(const float x = 4, const float y = 8)
		{
			// Animation
			b2Vec2 dir;
			if (isRunningRight)
				dir = b2Vec2(-x, -y);
			else
				dir = b2Vec2(x, -y);
			body->ApplyLinearImpulse(dir, body->GetWorldCenter(), true);
			stateCurrent = hurting;

			// Actually die
			isDead = true;
			b2Filter filter;
			filter.maskBits = BitNothing;
			for (auto fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
				fixture->SetFilterData(filter);
		}

		Vector2f getPosition() const
		{
			return position;
		}
		string getPositionString() const
		{
			return fmt::format("{:.0f} {:.0f}", position.x, position.y);
		}

		string getSpeedString() const
		{
			return fmt::format("{:.0f} {:.0f}", body->GetLinearVelocity().x * PixelsPerMeter, body->GetLinearVelocity().y * PixelsPerMeter);
		}
		b2Vec2 getVelocity() const
		{
			return this->body->GetLinearVelocity();
		}
		
		string getGroundedString() const
		{
			return isGrounded() ? "yes" : "no";
		}
		string getTouchingRoofString() const
		{
			return this->isTouchingRoof ? "yes" : "no";
		}
		void setTouchingRoof(const bool isTouchingRoof)
		{
			this->isTouchingRoof = isTouchingRoof;
		}
		bool isGrounded() const
		{
			if (isTouchingRoof)
				return false;

			if (stateCurrent == jumping || stateCurrent == falling)
			{
				if (getVelocity().y == 0)
					return true;
				return false;
			}
			return true;
		}
		bool runningRight() const
		{
			return this->isRunningRight;
		}

		string getNameOfState() const
		{
			switch (this->stateCurrent)
			{
				case standing: return "standing";
				case running:  return "running";
				case jumping:  return "jumping";
				case falling:  return "falling";
				case hurting:  return "hurting";
				case dead:     return "dead";
				default:       return "unknown";
			}
		}

		Vector2f getBodyPosition() const
		{
			return Vector2f(body->GetPosition().x * PixelsPerMeter, body->GetPosition().y * PixelsPerMeter);
		}

	private:
		void loadAnimations()
		{
			// Standing
			animStand.setSpriteSheet(tileset);
			animStand.addFrame(IntRect(0, 0, 64, 96));
			// Running (Only actual animation)
			animRun.setSpriteSheet(tileset);
			animRun.addFrame(IntRect(64, 0, 64, 96));
			animRun.addFrame(IntRect(128, 0, 64, 96));
			// Jumping
			animJump.setSpriteSheet(tileset);
			animJump.addFrame(IntRect(64 * 3, 0, 64, 96));
			// Falling (Same as standing)
			animStand.setSpriteSheet(tileset);
			animStand.addFrame(IntRect(0, 0, 64, 96));
			// Hurting
			animHurt.setSpriteSheet(tileset);
			animHurt.addFrame(IntRect(64 * 4, 0, 64, 96));
			// Dead (Same as hurting)
			animHurt.setSpriteSheet(tileset);
			animHurt.addFrame(IntRect(64 * 4, 0, 64, 96));

			// Ducking
			animDuck.setSpriteSheet(tileset);
			animDuck.addFrame(IntRect(64 * 5, 0, 64, 96));

			// Current animation
			animCurrent = &animStand;

			// Set origin as a semi temp fix things
			animation.setOrigin(32.0f, 0.f);
		}
		bool isFlipX() const
		{
			if (animation.getScale().x > 0)
				return false;
			return true;
		}

		void createEdgeShape(const b2Vec2 v1, const b2Vec2 v2, const uint16 categoryBits, const uint16 maskBits, const float friction, const bool isSensor) const
		{
			b2EdgeShape  shape;
			b2FixtureDef fixture;

			shape.SetTwoSided(v1, v2);
			fixture.shape = &shape;
			fixture.filter.categoryBits = categoryBits;
			fixture.filter.maskBits = maskBits;
			fixture.friction = friction;
			fixture.isSensor = isSensor;

			this->body->CreateFixture(&fixture);
		}
	};

	class Boss
	{
		enum State { normal, angry };
		State state;

		Texture tileset;
		Sprite sprNormal, sprAngry, *sprite;
		bool isRunningRight;
		Vector2f position; // Do we need to store spawn?
		b2Body *body;

		float damageForce;

		void loadSprites()
		{
			sprNormal.setTexture(tileset);
			sprNormal.setTextureRect(IntRect(0, 0, 64, 92));
			sprNormal.setOrigin(32, 46);
			sprNormal.setColor(Color(255, 255, 255, 255 * 0.9));
			sprAngry.setTexture(tileset);
			sprAngry.setTextureRect(IntRect(64, 0, 64, 92));
			sprAngry.setOrigin(32, 46);
			sprAngry.setColor(Color(255, 255, 255, 255 * 0.9));
		}

		bool isFlipX() const
		{
			if (sprite->getScale().x > 0)
				return false;
			return true;
		}

	public:
		// Follow is the new mode, classic is the same as KraXmas 1
		enum Type { follow, classic };
		Type type;
		int health;

		Boss(const Vector2f spawn, b2World *world, Texture* tileset)
		{
			// Local stuff
			this->tileset        = *tileset;
			this->isRunningRight = false;
			this->sprite         = &sprAngry;
			this->position       = spawn;
			this->health         = 8;

			// Load stuff
			loadSprites();

			// Box2D
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody; // Dynamic (We can apply linear force) or Kinematic (No gravity)?
			bodyDef.position.Set(spawn.x / PixelsPerMeter, spawn.y / PixelsPerMeter);
			bodyDef.fixedRotation = true;
			bodyDef.gravityScale = 0.f;
			body = world->CreateBody(&bodyDef);

			b2PolygonShape  shape;
			shape.SetAsBox(32 / PixelsPerMeter, 46 / PixelsPerMeter);
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.isSensor = true;
			fixtureDef.filter.categoryBits = BitBoss;
			fixtureDef.filter.maskBits = BitPlayer;
			body->CreateFixture(&fixtureDef);
		}
		void move(const float x, const float y) const
		{
			body->SetLinearVelocity(b2Vec2(x, y));
		}
		void setAngry(const bool isAngry)
		{
			if (isAngry)
				sprite = &sprAngry;
			else
				sprite = &sprNormal;
		}
		void update()
		{
			position = Vector2f(body->GetPosition().x * PixelsPerMeter + 32, body->GetPosition().y * PixelsPerMeter + 32);
		}
		Sprite* getSprite() const
		{
			// See if we need to flip
			if (body->GetLinearVelocity().x < 0 && !isFlipX())
				sprite->setScale(-1, 1);
			else if (body->GetLinearVelocity().x > 0 && isFlipX())
				sprite->setScale(1, 1);

			sprite->setPosition(position);
			return sprite;
		}
		Vector2f getPosition() const
		{
			return position;
		}
		Vector2f getBodyPosition() const
		{
			return Vector2f(body->GetPosition().x * PixelsPerMeter, body->GetPosition().y * PixelsPerMeter);
		}
		void die() const
		{
			b2Filter filter;
			filter.maskBits = BitNothing;
			for (auto fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
				fixture->SetFilterData(filter);
		}
	};

	#pragma endregion 

	#pragma region --- Scene manager ---

	// Different scenes
	enum class SceneType { menu, level, dead };

	// Class for scenes
	class Game;

	class Scene
	{
	public:
		//Scene();
		virtual ~Scene() {}
		virtual void create() {}
		virtual void render(const Time frameTime) {}
		virtual void destroy() {}
		virtual void onEvent(const Event ev) {}
	};

	#pragma endregion

	#pragma region --- Engine config ---

	class Config
	{
	public:
		// Windowed (prob without border), borderless fullscreen and normal fullscreen
		//enum WindowType { windowed, borderless, fullscreen };

		// Music volume, 0-100
		int musicVolume;
		// Sound volume, 0-100
		int soundVolume;
		// MSAA samples
		int antialias;
		// Fullscreen mode, true/false
		bool fullscreen;
		// Max fps allowed
		int maxFps;
		// vSynx, true/false
		bool vSync;
		// Window scale (from 1280x720)
		double windowScale;

		// Keybindings
		class Controls
		{ public: int left, right, jump; };
		Controls keyboard, controller;

		// Window size
		class Window
		{ public: int width, height; };
		Window window;

		Config()
		{
			// Default config
			this->musicVolume = 80;
			this->soundVolume = 80;
			this->antialias   = 0;		// Unused
			this->fullscreen  = false;	// Unused, max windowScale is fullscreen
			this->maxFps      = 60;		// Unused, for now
			this->vSync       = false;	// Unused, always disabled
			this->windowScale = 1.0;

			// Default keyboard bindings
			this->keyboard.left  = static_cast<int>(Keyboard::Left);
			this->keyboard.right = static_cast<int>(Keyboard::Right);
			this->keyboard.jump  = static_cast<int>(Keyboard::Up);

			// Default controller bindings
			// Left and right are the axis used
			// Jump is the button used
			// +0 = right, -0 = left, -1 = up, +1 = down
			// Xbox controller also uses 6 for l/r and 7 for u/d on dpad
			this->controller.left  = 0;
			this->controller.right = 0;
			this->controller.jump  = 0;
		}

		bool loadFromFile()
		{
			fstream file;
			file.open(Engine::getWritePath("config"), ios::in | ios::binary);
			if (!file.good())
				return false;
			file.read(reinterpret_cast<char*>(&*this), sizeof(Config));
			file.close();
			return true;
		}
		void saveToFile() const
		{
			fstream file;
			file.open(Engine::getWritePath("config"), ios::out | ios::binary);
			file.write(reinterpret_cast<const char*>(&*this), sizeof(Config));
			file.close();
		}
	};

	#pragma endregion

	#pragma region --- Save manager ---

	class Save
	{
	public:
		int level, lives, coins;
		// v2
		float totalTime;
		int   totalDeaths;

		Save()
		{
			this->level = 0;
			this->lives = 5;
			this->coins = 0;

			this->totalTime   = 0.f;
			this->totalDeaths = 0;
		}

		Save(const string fileName)
		{
			fstream file;
			file.open(Engine::getWritePath(fileName), ios::in | ios::binary);
			file.read(reinterpret_cast<char*>(&*this), sizeof(Save));
			file.close();
		}

		void save(const string fileName) const
		{
			fstream file;
			file.open(Engine::getWritePath(fileName), ios::out | ios::binary);
			file.write(reinterpret_cast<const char*>(&*this), sizeof(Save));
			file.close();
		}
	};

	#pragma endregion 

	#pragma region --- Game container ---

	class Game
	{
	public:
		Window     *window;
		Audio      *audio;
		Config     *config;
		FileStream *fileStream;
		Scene      *currentScene = nullptr;

		class Scenes
		{
		public:
			Scene *menu    = nullptr;
			Scene *level   = nullptr;
			Scene *submenu = nullptr;
			Scene *endgame = nullptr;
			Scene *dummy   = nullptr;
		};
		Scenes scenes;
		SceneHUD *hud = nullptr;

		Level *levels[10];
		Save  *save;

		bool devMode = false;
	};
	#pragma endregion 

	#pragma region --- Input manager ---
	class Input
	{
		// Joystick deadzone
		float deadzone = 25.f;

	public:
		enum Key { up, down, left, right, jump, pause, restart, ok, back };

		bool isPressed(const Key key) const
		{
			switch (key)
			{
				case up:      return Keyboard::isKeyPressed(Keyboard::Up)     || isAxisUp();
				case down:    return Keyboard::isKeyPressed(Keyboard::Down)   || isAxisDown();
				case left:    return Keyboard::isKeyPressed(Keyboard::Left)   || isAxisLeft();
				case right:   return Keyboard::isKeyPressed(Keyboard::Right)  || isAxisRight();
				case jump:    return Keyboard::isKeyPressed(Keyboard::Up)     || Joystick::isButtonPressed(0, 0);
				case pause:   return Keyboard::isKeyPressed(Keyboard::Escape) || Joystick::isButtonPressed(0, 7);
				case restart: return Keyboard::isKeyPressed(Keyboard::R)      || Joystick::isButtonPressed(0, 3);
				case ok:      return Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0);
				case back:    return Keyboard::isKeyPressed(Keyboard::M)      || Joystick::isButtonPressed(0, 6);
			}
			return false;
		}

		static int getJoystickButton(const Key key)
		{
			switch (key)
			{
				case jump:    return 0;
				case pause:   return 7;
				case restart: return 3;
				case ok:      return 0;
				case back:    return 6;
				default:      return -1;
			}
		}

		/*
		 *			Keyboard	Controller
		 * Pause	Esc			7: Start
		 * Restart	R			2: Y
		 * OK		Return		0: A
		 * Back		M			6: Select
		 */

		/// @brief Check if controller is connected for player 1
		static bool isUsingController()
		{
			// Simple, at least for now
			return Joystick::isConnected(0);
		}

	private:
		bool isAxisLeft() const
		{
			if (!isUsingController())
				return false;
			return getXAxisPosition() < -deadzone;
		}
		bool isAxisRight() const
		{
			if (!isUsingController())
				return false;
			return getXAxisPosition() > deadzone;
		}
		bool isAxisUp() const
		{
			if (!isUsingController())
				return false;
			return getYAxisPosition() < -deadzone;
		}
		bool isAxisDown() const
		{
			if (!isUsingController())
				return false;
			return getYAxisPosition() > deadzone;
		}

		float getXAxisPosition() const
		{
			const auto joystick = Joystick::getAxisPosition(0, Joystick::X);
			const auto dpad     = Joystick::getAxisPosition(0, Joystick::PovX);

			if (joystick > deadzone || joystick < -deadzone)
				return joystick;
			return dpad;
		}
		float getYAxisPosition() const
		{
			const auto joystick = Joystick::getAxisPosition(0, Joystick::Y);
			const auto dpad     = Joystick::getAxisPosition(0, Joystick::PovY);

			if (joystick > deadzone || joystick < -deadzone)
				return joystick;
			return dpad;
		}
	};
	#pragma endregion
}