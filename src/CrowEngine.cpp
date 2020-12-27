#include "CrowEngine.hpp"

class SceneHUD;

using namespace std;
using namespace sf;

namespace ce
{
	Engine::Engine(const bool devMode)
	{
		// Allegro
		al_init();
		if (!al_install_audio())
			error("Error installing audio");
		if (!al_init_acodec_addon())
			error("Error installing audio codecs");
		if (!al_reserve_samples(1))
			error("Error reserving audio samples");
		//al_init_native_dialog_addon();

		// Load text log
		// Change for release
		//if (devMode || Keyboard::isKeyPressed(Keyboard::LControl))
		//	textLog = al_open_native_text_log("CrowEngine2 Log", ALLEGRO_TEXTLOG_NO_CLOSE | ALLEGRO_TEXTLOG_MONOSPACE);

		// Create config dir
		filesystem::create_directories(getWritePath(""));

		// PhysFS
		if (!PHYSFS_init(nullptr))
			error("Failed to load taco manager");
		else if (!PHYSFS_mount("data.taco", nullptr, true))
			error("Failed to load taco");
		else
			al_set_physfs_file_interface();

		// Debug info
		if (devMode)
		{
			// This assumes we have already created a window
			dev("Engine is running in developer mode");
#ifdef CE_DEBUG
			dev(fmt::format("Using {} {}", glGetString(GL_VENDOR), glGetString(GL_RENDERER)));
			dev(fmt::format("Using OpenGL {}", glGetString(GL_VERSION)));
#endif
		}
	}

	void Engine::log(const string message)
	{
		cout << "[INFO] " << message << endl;
	}

	void Engine::error(const string message)
	{
        cout << "[ERR ] " << message << endl;
		exit(-1);
	}

	void Engine::warning(const string message)
	{
        cout << "[WARN] " << message << endl;
	}

	void Engine::dev(const string message)
	{
		cout << "[DEV ] " << message << endl;
	}

	string Engine::getWritePath(const string file)
	{
		#if defined CE_WINDOWS
			char* buf = nullptr;
			_dupenv_s(&buf, nullptr, "APPDATA");
			_mkdir(fmt::format("{}\\CrowGames", buf).c_str()); // Just to be sure and to fix some bugs
			return fmt::format("{}\\CrowGames\\{}\\{}", buf, ProjectName, file);
		#elif defined CE_LINUX
			return fmt::format("~/.config/CrowGames/{}/{}", ProjectName, file);
        #else
			return "/";
		#endif
	}

	bool Engine::fileExists(const string fileName)
	{
		fstream file(Engine::getWritePath(fileName));
		const auto exists = file.good();
		file.close();
		return exists;
	}
}