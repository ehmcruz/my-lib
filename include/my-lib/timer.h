#ifndef __MY_LIB_TIMER_HEADER_H__
#define __MY_LIB_TIMER_HEADER_H__

#include <vector>
#include <algorithm>
#include <memory>
#include <utility>
#include <variant>

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

template <typename Tget_current_time, typename Talloc=std::allocator<int>>
class Timer
{
public:
	using Ttime = decltype(std::declval<Tget_current_time>()());

	struct Event {
		Ttime time;
		bool re_schedule;
	};

	struct Descriptor {
		Event *ptr;
	};

private:
	using TimerCallback = Callback<Event>;

	struct FreeHandler {
		virtual void free_memory (Timer *timer, TimerCallback *ptr) = 0;
	};

	struct EventFull : public Event {
		TimerCallback *callback;
		FreeHandler *free_handler;

		bool enabled;

		inline bool operator< (const EventFull& rhs) const
		{
			return (this->time > rhs.time);
		}
	};

	using TallocEventFull = typename std::allocator_traits<Talloc>::template rebind_alloc<EventFull>;
	TallocEventFull event_allocator;
	std::vector<EventFull*> events; // we let the vector use its standard allocator
	Tget_current_time get_current_time_;

public:
	Timer (Tget_current_time get_current_time__)
		: get_current_time_(get_current_time__)
	{
	}

	Timer (Tget_current_time get_current_time__, const Talloc& allocator__)
		: get_current_time_(get_current_time__), event_allocator(allocator__)
	{
	}

	~Timer ()
	{
		for (auto *event : this->events)
			this->destroy_event(event);
	}

	inline Ttime get_current_time () const
	{
		return this->get_current_time_();
	}

	inline uint32_t get_n_scheduled_events () const
	{
		return this->events.size();
	}

	void trigger_events ()
	{
		const Ttime time = this->get_current_time();

		while (!this->events.empty()) {
			EventFull *event = this->events.front();

			if (event->time <= time) {
				this->pop();

				auto& c = *(event->callback);

				event->re_schedule = false;

				if (event->enabled)
					c(*event);

				if (event->re_schedule)
					this->push(event);
				else
					this->destroy_event(event);
			}
			else
				break;
		}
	}

	/*
		When creating the event listener by r-value ref,
		we allocate internal storage and copy the value to it.
	*/
	template <typename Tcallback>
	Descriptor schedule_event (const Ttime& time, const Tcallback& callback)
		//requires std::is_rvalue_reference<decltype(callback)>::value
	{
		using Tc = Tcallback;
		using TallocTc = typename std::allocator_traits<TallocEventFull>::template rebind_alloc<Tc>;

		struct MyFreeHandler : public FreeHandler {
			virtual void free_memory (Timer *timer, TimerCallback *ptr) override
			{
				TallocTc callback_allocator(timer->event_allocator);
				callback_allocator.deallocate(static_cast<Tc*>(ptr), 1);
			}
		};

		static MyFreeHandler my_free_handler;

		TallocTc callback_allocator(this->event_allocator);

		Tc *persistent_callback = new (callback_allocator.allocate(1)) Tc(callback);
		
		EventFull *event = new (this->event_allocator.allocate(1)) EventFull;
		event->time = time;
		event->callback = persistent_callback;
		event->free_handler = &my_free_handler;
		event->enabled = true;

		this->push(event);
		
		return Descriptor { .ptr = event };
	}

	inline void unschedule_event (Descriptor& descriptor)
	{
		EventFull *event = static_cast<EventFull*>(descriptor.ptr);
		event->enabled = false; // better than rebuild the heap
	}

private:
	inline void push (EventFull *event)
	{
		this->events.push_back(event);
		std::push_heap(this->events.begin(), this->events.end());
	}

	inline void pop ()
	{
		std::pop_heap(this->events.begin(), this->events.end());
		this->events.pop_back();
	}

	inline void destroy_event (EventFull *event)
	{
		event->free_handler->free_memory(this, event->callback);
		this->event_allocator.deallocate(event, 1);
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif