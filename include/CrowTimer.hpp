#pragma once

/*
 * ?: start()
 * ?: stop()
 * ?: pause()
 */

namespace ce
{

	class Timer
	{
		unsigned long time;
	public:
		Timer()
		{
			this->time = 0;
		}

		void tick(const int microSeconds)
		{
			this->time += microSeconds;
		}
		void restart()
		{
			this->time = 0;
		}

		unsigned long getTime() const
		{
			return this->time;
		}
		float getTimeSeconds() const
		{
			return this->time / 1000000.f;
		}
	};
}