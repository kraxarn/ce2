#pragma once

#include <allegro5/allegro.h>

namespace ce
{
	class Timer
	{
		ALLEGRO_TIMER       *timer;
		ALLEGRO_EVENT_QUEUE *queue;

	public:
		Timer()
		{
			this->timer = al_create_timer(1000.f);

			this->queue = al_create_event_queue();
			al_register_event_source(this->queue, al_get_timer_event_source(this->timer));
		}
		~Timer()
		{
			al_destroy_timer(this->timer);
			al_destroy_event_queue(this->queue);
		}

		void start() const
		{
			al_start_timer(this->timer);
		}
		void resume() const
		{
			al_resume_timer(this->timer);
		}
		void stop() const
		{
			al_stop_timer(this->timer);
		}

		bool started() const
		{
			return al_get_timer_started(this->timer);
		}

		int64_t getCount() const
		{
			return al_get_timer_count(this->timer);
		}
		void setCount(const int64_t newCount) const
		{
			al_set_timer_count(this->timer, newCount);
		}
		void addCount(const int64_t diff) const
		{
			al_add_timer_count(this->timer, diff);
		}

		// Probably not needed
		double getSpeed() const
		{
			return al_get_timer_speed(this->timer);
		}
		void setSpeed(const double newSpeed) const
		{
			al_set_timer_speed(this->timer, newSpeed);
		}
	};
}
