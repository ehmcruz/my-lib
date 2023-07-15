#ifndef __MY_LIBS_TIMER_HEADER_H__
#define __MY_LIBS_TIMER_HEADER_H__

#include <iostream>
#include <vector>
#include <queue>
#include <functional>

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
	using Descriptor = uint64_t;
	
	struct Event {
		Descriptor id;
		Ttime time;
		bool re_schedule;
	};

private:
	struct EventFull {
		Descriptor id; // we identify by ids because priority_queues are not pointer-identifiable
		Ttime time;
		Callback<Event> *callback;
		bool enabled;
		bool auto_destroy;

		inline Event to_Event () const
		{
			return Event { .id = this->id, .time = this->time };
		}

		inline bool operator> (const EventFull& rhs) const
		{
			return (this->time > rhs.time);
		}
	};

	std::priority_queue<EventFull, std::vector<EventFull>, std::greater<EventFull>> events;
	Ttime current_time;
	Descriptor next_id;

public:
	Timer (const Ttime initial_time)
		: current_time(initial_time)
	{
		this->next_id = 0;
	}

	inline uint32_t get_n_scheduled_events () const
	{
		return this->events.size();
	}

	void trigger_events (const Ttime time)
	{
		this->current_time = time;

		while (!this->events.empty()) {
			const EventFull& event_ = this->events.top();

			if (event_.time <= time) {
				EventFull event = event_; // copy before deleting

				this->events.pop();

				auto& c = *(event.callback);

				Event user_event = event.to_Event();
				user_event.re_schedule = false;

				if (event.enabled)
					c(user_event);

				if (user_event.re_schedule) {
					event.time = user_event.time;
					this->events.push(event);
				}
				else if (event.auto_destroy)
					delete event.callback;
			}
			else
				break;
		}
	}

	/* When creating the event listener by r-value ref,
	   we allocate internal storage and copy the value to it.
	*/
	template <typename Tcallback>
	Descriptor schedule_event (const Ttime time, Tcallback&& callback)
		requires std::is_rvalue_reference<decltype(callback)>::value
	{
		using Tc = Mylib::remove_type_qualifiers< decltype(callback) >::type;

		Tc *persistent_callback = new Tc(callback);
		
		EventFull event {
			.id = this->next_id++,
			.time = time,
			.callback = persistent_callback,
			.enabled = true,
			.auto_destroy = true
		};

		this->events.push(event);
		
		return event.id;
	}

	void unschedule_event (const Descriptor& descriptor)
	{
/*		for (const auto& event : this->events) {
			if (event.id == descriptor)
				event.enabled = false; // better than rebuild the heap
		}*/
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif