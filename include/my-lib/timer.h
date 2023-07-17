#ifndef __MY_LIBS_TIMER_HEADER_H__
#define __MY_LIBS_TIMER_HEADER_H__

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

#include <cstdint>
#include <cstdlib>

#include <my-lib/macros.h>
#include <my-lib/std.h>
#include <my-lib/trigger.h>

namespace Mylib
{
namespace Trigger
{

// ---------------------------------------------------

template <typename Ttime>
class Timer
{
public:
	struct Event {
		Ttime time;
		bool re_schedule;
	};

	struct EventFull {
		Ttime time;
		Callback<Event> *callback;
		bool enabled;

		inline Event to_Event () const
		{
			return Event { .time = this->time };
		}
	};

	struct Descriptor {
		EventFull *ptr;
	};

private:
	struct Internal {
		EventFull *event_full;

		inline bool operator< (const Internal& rhs) const
		{
			return (this->event_full->time > rhs.event_full->time);
		}
	};

	std::vector<Internal> events;
	Ttime current_time;

public:
	Timer (const Ttime& initial_time)
		: current_time(initial_time)
	{
	}

	inline uint32_t get_n_scheduled_events () const
	{
		return this->events.size();
	}

	void trigger_events (const Ttime& time)
	{
		this->current_time = time;

		while (!this->events.empty()) {
			EventFull *event = this->events.front().event_full;

			if (event->time <= time) {
				this->pop();

				auto& c = *(event->callback);

				Event user_event = event->to_Event();
				user_event.re_schedule = false;

				if (event->enabled)
					c(user_event);

				if (user_event.re_schedule) {
					event->time = user_event.time;
					this->push(event);
				}
				else {
					delete event->callback;
					delete event;
				}
			}
			else
				break;
		}
	}

	/* When creating the event listener by r-value ref,
	   we allocate internal storage and copy the value to it.
	*/
	template <typename Tcallback>
	Descriptor schedule_event (const Ttime& time, const Tcallback& callback)
		//requires std::is_rvalue_reference<decltype(callback)>::value
	{
		using Tc = Mylib::remove_type_qualifiers< decltype(callback) >::type;

		Tc *persistent_callback = new Tc(callback);
		
		EventFull *event = new EventFull {
			.time = time,
			.callback = persistent_callback,
			.enabled = true,
		};

		this->push(event);
		
		return Descriptor { .ptr = event };
	}

	void unschedule_event (const Descriptor& descriptor)
	{
		descriptor.ptr->enabled = false; // better than rebuild the heap
	}

private:
	inline void push (EventFull *event)
	{
		Internal storage { .event_full = event };
		this->events.push_back(storage);
		std::push_heap(this->events.begin(), this->events.end());
	}

	inline void pop ()
	{
		std::pop_heap(this->events.begin(), this->events.end());
		this->events.pop_back();
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif