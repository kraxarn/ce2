#pragma once

#include <iostream>
#include <physfs.h>
using namespace std;

#include <SFML/System/Vector2.hpp>
using namespace sf;

namespace ce
{
	#pragma region Level types
	enum class LevelType { stage, boss };
	#pragma endregion 

	#pragma region Level class
	class Level
	{
	public:
		LevelType	type;
		string		file;
		Vector2f	spawn;
		string		music;
		string		name;

		class LevelFile
		{
		public:
			// Version of clf
			// Default: 2
			int version;
			// Background image to use from image/
			// Default: snow.png
			char background[16];
			// Boss or normal stage
			// Default: false
			bool isBoss;
			// Map size in tiles
			// Default: 75, 25
			int mapSize[2];
			// Tile size
			// Default: 64
			int tileSize;

			// Main tileset (1-49)
			// Default: snow.png
			char mainTileset[16];
			// Item tileset (50+)
			// Default: item.png
			char itemTileset[16];

			// Main array for level
			int map[75][25] = { 0 };
		};

		/// @brief Creates a level and loads map from level folder
		/// @param type Level type, either stage or boss
		/// @param file File to load
		/// @param music Music file to use
		/// @param name Name of stage to display
		Level(const LevelType type, const string file, const string music, const string name)
		{
			this->type = type;
			this->file = file;
			//this->spawn = spawn;
			this->music = music;
			this->name = name;

			// Default to 0,0 instead of requiring spawn
			// Spawn is set in the level either way
			this->spawn = Vector2f(0, 0);
		}
		Level()
		{
			// Just some default/dummy stuff
			this->type  = LevelType::stage;
			this->file  = "dummy.clf";
			this->music = "dummy.xm";
			this->name  = "Dummy Level";
		}

		/// @brief Loads level from file
		/// @param level LevelFile to load to
		/// @param fileName File to load from in level/
		/// @return If loading was successful
		static bool loadLevel(LevelFile *level, const string fileName)
		{
			// c-style reading since physfs is c

			// Open file in taco
			auto *file = PHYSFS_openRead(("level/" + fileName).c_str());
			if (file == nullptr)
				return false;
			// Read from file
			const auto read = PHYSFS_readBytes(file, level, sizeof(LevelFile));
			if (read != sizeof(LevelFile))
				return false;
			// If no errors, return true
			return true;
		}
	};
	#pragma endregion
}