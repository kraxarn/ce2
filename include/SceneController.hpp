
#ifndef SCENECONTROLLER_HPP
#define SCENECONTROLLER_HPP

#include "CrowEngine.hpp"

using namespace ce;

class SceneController : public Scene
{
	static void createText(Text* text, const String string, const Font* font, const int size, const int style, const Color color, const Vector2f position)
	{
		text->setString(string);
		text->setFont(*font);
		text->setCharacterSize(size);
		text->setStyle(style);
		text->setFillColor(color);
		text->setPosition(position);
	}

	enum CurrentButton { none, left, right, jump };

	Game *game;
	RenderWindow *window;
	View *view;

	Font fontMain, fontInfo;
	Text textMain, textInfo, textControllerName, textOptions;

	Texture texController;
	Sprite  sprController;

	CurrentButton current;
	
public:
	SceneController::SceneController(Game *game)
	{
		this->window = game->window->getWindow();
		this->view   = game->window->getView();
		this->game   = game;

		// Fonts
		fontMain.loadFromFile("font/menu.ttf");
		fontInfo.loadFromFile("font/debug.ttf");

		// Textures
		texController.loadFromFile("image/controller.png");

		// Sprites
		sprController.setTexture(texController);
		sprController.setOrigin(134, 100);
		sprController.setPosition(1280 / 2, 720 / 2 - 128);

		// Create text
		createText(
			&textMain,
			"Controller Detected!",
			&fontMain,
			32,
			Text::Regular,
			Color(255, 255, 255),
			Vector2f(1280 / 2, 720 / 2)
		);
		textMain.setOrigin(textMain.getLocalBounds().width / 2, textMain.getLocalBounds().height / 2);

		createText(
			&textControllerName,
			Joystick::getIdentification(0).name,
			&fontInfo,
			18,
			Text::Regular,
			Color(255, 255, 255),
			Vector2f(1280 / 2, 720 / 2 + 32)
		);
		textControllerName.setOrigin(textControllerName.getLocalBounds().width / 2, 0);

		createText(
			&textInfo,
			"Would you like to configure it now?",
			&fontInfo,
			22,
			Text::Regular,
			Color(255, 255, 255),
			Vector2f(1280 / 2, 720 / 2 + 52)
		);
		textInfo.setOrigin(textInfo.getLocalBounds().width / 2, 0);

		createText(
			&textOptions,
			"< Yes >",
			&fontInfo,
			28,
			Text::Regular,
			Color(255, 255, 255),
			Vector2f(1280 / 2, 720 / 2 + 82)
		);
		textOptions.setOrigin(textOptions.getLocalBounds().width / 2, 0);

		// Vars
		current = none;
	}

	void onEvent(const Event ev) override
	{
		if (ev.type == Event::KeyPressed)
		{
			if (ev.key.code == Keyboard::Left || ev.key.code == Keyboard::Right)
			{
				if (textOptions.getString() == "< Yes >")
					textOptions.setString("< No >");
				else
					textOptions.setString("< Yes >");
			}
			else if (ev.key.code == Keyboard::Return && current == none)
			{
				if (textOptions.getString() == "< Yes >")
				{
					textOptions.setString("Press button for: left");
					current = left;
				}
				else
					game->currentScene = game->scenes.menu;
			}
		}
		else if (ev.type == Event::JoystickButtonPressed)
		{
			Engine::log(fmt::format("Button pressed: {}", ev.joystickButton.button));

			switch (current)
			{
				case left:
					game->config->controller.left = ev.joystickButton.button;
					textOptions.setString("Press button for: right");
					break;
				case right:
					game->config->controller.right = ev.joystickButton.button;
					textOptions.setString("Press button for: jump");
					break;
				case jump:
					game->config->controller.jump = ev.joystickButton.button;
					game->currentScene = game->scenes.menu;
					break;
				default:
					break;
			}
		}
		else if (ev.type == Event::JoystickMoved)
		{
			if (ev.joystickMove.position > 90 || ev.joystickMove.position < -90)
				Engine::log(fmt::format("Joystick {} moved: {}", ev.joystickMove.axis, ev.joystickMove.position));
		}
	}

	void render(const Time frameTime) override
	{
		window->clear(Color(51, 153, 218));

		// Fuck it
		textOptions.setOrigin(textOptions.getLocalBounds().width / 2, 0);

		if (current == none)
		{
			window->draw(textMain);
			window->draw(textControllerName);
			window->draw(textInfo);
		}
		window->draw(sprController);
		window->draw(textOptions);
	}

	void destroy() override
	{
		
	}
};

#endif // SCENECONTROLLER_HPP