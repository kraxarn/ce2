#pragma once

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <fmt/format.h>

class Audio
{
	ALLEGRO_AUDIO_STREAM *music;

	string currentMusic;
	float  musicVolume;
	float  soundVolume;

	ALLEGRO_SAMPLE          *samples[6];
	ALLEGRO_SAMPLE_INSTANCE *sampleInstances[6];

	string soundFiles[6] = { "boss", "coin", "complete", "fall", "jump", "present" };
public:
	enum SoundFile { boss, coin, complete, fall, jump, present };

	Audio(const float musicVolume, const float soundVolume)
	{
		// Reset music stream
		music = nullptr;

		// Set volumes
		this->musicVolume = musicVolume;
		this->soundVolume = soundVolume;

		if (!al_is_audio_installed())
			cout << "Error: Audio isn't installed" << endl;

		// Load all the sounds
		loadSounds();
	}

	void loadSounds()
	{
		for (auto i = 0; i < 6; i++)
		{
			samples[i] = al_load_sample(fmt::format("sound/{}.wav", soundFiles[i]).c_str());
			sampleInstances[i] = al_create_sample_instance(samples[i]);
			al_attach_sample_instance_to_mixer(sampleInstances[i], al_get_default_mixer());
		}
	}

	/// @brief Plays and loops file in music folder
	/// @return If loading was successfull
	/// @param file File to load
	bool playMusic(const char* file)
	{
		if (currentMusic == string(file))
			return true;

		if (file[0] == '\0')
			return false;

		if (music)
		{
			al_set_audio_stream_playing(music, false);
			al_destroy_audio_stream(music);
			music = nullptr;
		}
		music = al_load_audio_stream(fmt::format("music/{}", file).c_str(), 3, 1024);
		// Check if load was successfull
		if (!music)
			return false;
		currentMusic = file;
		al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
		al_set_audio_stream_gain(music, musicVolume / 100);
		al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
		return true;
	}
	/// @brief Simple wrapper around playMusic(char*)
	bool playMusic(string file)
	{
		return playMusic(file.c_str());

		// We don't need a char*, only a const char*
		//const auto cstr = &file[0u];
		//return playMusic(cstr);
	}

	/// @brief Gets the current playing track (without extension)
	string getCurrentMusic() const
	{
		if (currentMusic.empty())
			return "none";
		return currentMusic.substr(0, currentMusic.find('.'));
	}

	/// @brief Plays a pre-loaded sound
	/// @return If successfull
	/// @param index Sound to play
	bool playSound(const SoundFile index)
	{
		// Set volume here or in setSoundVolume?
		al_set_sample_instance_gain(sampleInstances[index], soundVolume / 100);
		return al_play_sample_instance(sampleInstances[index]);
	}

	void setMusicVolume(const float volume)
	{
		musicVolume = volume;
		if (music)
			al_set_audio_stream_gain(music, musicVolume / 100);
	}
	void setSoundVolume(const float volume)
	{
		// We just need to update our variable
		soundVolume = volume;
	}
	/// @brief Temporarily lower the volume
	/// @param amount Amount (0-1)
	void lowerVolume(const float amount) const
	{
		if (music)
			al_set_audio_stream_gain(music, (musicVolume / 100) * amount);
	}
	
	int getMusicLength() const
	{
		if (music)
			return al_get_audio_stream_length_secs(music);
		return 0;
	}
	int getMusicPosition() const
	{
		if (music)
			return al_get_audio_stream_position_secs(music);
		return 0;
	}
};