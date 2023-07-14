#ifndef __MY_LIBS_TIMER_HEADER_H__
#define __MY_LIBS_TIMER_HEADER_H__

#include <iostream>
#include <vector>
#include <queue>
#include <functional>

#include <stdint.h>
#include <stdlib.h>

#include <my-lib/macros.h>
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
	};

private:
	struct EventFull {
		Descriptor id; // we identify by ids because priority_queues are not pointer-identifiable
		Ttime time;
		Callback<Event>& callback;
		bool enabled;
		bool auto_destroy;

		Event to_Event () const
		{
			return Event { .id = this->id, .time = this->time };
		}
	};

	std::priority_queue<EventFull> events;
	Ttime current_time;
	Descriptor next_id;

public:
	Timer (Ttime initial_time = 0)
		: current_time(initial_time)
	{
		this->next_id = 0;
	}

	inline uint32_t get_n_scheduled_events ()
	{
		return this->events.size();
	}

	void trigger_events (Ttime time)
	{
		this->current_time = time;

		while (!this->events.empty()) {
			EventFull& event = this->events.top();

			if (event.time >= time) {
				event.callback( event.to_Event() );

				if (event.auto_destroy)
					delete &event.callback;
				
				this->events.pop();
			}
			else
				break;
		}
	}

	/* When creating the event listener by r-value ref,
	   we allocate internal storage and copy the value to it.
	*/
	template <typename Tcallback>
	Descriptor schedule_event (Ttime time, Tcallback&& callback)
		requires std::is_rvalue_reference<decltype(callback)>::value
	{
		using Tc = Mylib::remove_type_qualifiers< decltype(callback) >::type;

		Tc *persistent_callback = new Tc(callback);
		
		EventFull event {
			.id = this->next_id++;
			.time = time;
			.callback = *persistent_callback,
			.enabled = true,
			.auto_destroy = true
		};

		this->events.push(event);
		
		return event.id;
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif