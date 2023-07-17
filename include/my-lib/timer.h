#ifndef __MY_LIBS_TIMER_HEADER_H__
#define __MY_LIBS_TIMER_HEADER_H__

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>

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

template <typename Ttime, typename Talloc=std::allocator<int>>
class Timer
{
public:
	struct Event {
		Ttime time;
		bool re_schedule;
	};

	using TimerCallback = Callback<Event>;

	struct CallbackHandler {
		virtual void free_memory (TimerCallback *ptr) = 0;
	};

	struct EventFull {
		Ttime time;
		TimerCallback *callback;
		CallbackHandler *callback_handler;
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

	using TallocEventFull = typename std::allocator_traits<Talloc>::rebind_alloc<EventFull>;
	TallocEventFull allocator;
	std::vector<Internal> events; // we let the vector use its standard allocator
	Ttime current_time;

public:
	Timer (const Ttime& initial_time)
		: current_time(initial_time)
	{
	}

	Timer (const Ttime& initial_time, const Talloc& allocator_)
		: current_time(initial_time), allocator(allocator_)
	{
	}

	~Timer ()
	{
		for (auto& data : this->events)
			this->destroy_event(data.event_full);
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
				else
					this->destroy_event(event);
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
		using Tc = Tcallback;
		using TallocTc = typename std::allocator_traits<TallocEventFull>::rebind_alloc<Tc>;

		struct MyCallbackHandler : public CallbackHandler {
			TallocTc allocator;

			MyCallbackHandler (const TallocEventFull& allocator_)
				: allocator(allocator_)
			{
			}

			virtual void free_memory (TimerCallback *ptr) override
			{
				this->allocator.deallocate(static_cast<Tc*>(ptr), 1);
			}
		};

		static MyCallbackHandler *saved = nullptr;

		if (saved == nullptr)
			saved = new MyCallbackHandler(this->allocator);

		Tc *persistent_callback = new (saved->allocator.allocate(1)) Tc(callback);
		
		EventFull *event = new (this->allocator.allocate(1)) EventFull {
			.time = time,
			.callback = persistent_callback,
			.callback_handler = saved,
			.enabled = true
		};

		this->push(event);
		
		return Descriptor { .ptr = event };
	}

	inline void unschedule_event (const Descriptor& descriptor)
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

	inline void destroy_event (EventFull *event)
	{
		event->callback_handler->free_memory(event->callback);
		this->allocator.deallocate(event, 1);
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif