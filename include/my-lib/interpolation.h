#ifndef __MY_LIB_INTERPOLATION_HEADER_H__
#define __MY_LIB_INTERPOLATION_HEADER_H__

#include <variant>

#include <my-lib/std.h>
#include <my-lib/trigger.h>
#include <my-lib/coroutine.h>
#include <my-lib/memory.h>


namespace Mylib
{

// ---------------------------------------------------

/*
	Tx is the type of the x-axis.
	For instance, can represent the time required for the interpolation.
*/

template <typename Tx>
class Interpolator
{
private:
	Tx x; // Current x-axis value. Varies from 0 to max_x.
	Tx max_x;

public:
	Interpolator (const Tx max_x_)
		: x(0), max_x(max_x_)
	{
	}

	virtual ~Interpolator () = default;

	// Returns true if the interpolation is not finished.
	// Returns false otherwise.

	bool operator() (const Tx delta_x)
	{
		this->x += delta_x;

		if (this->x > this->max_x) [[unlikely]]
			this->x = this->max_x;
		
		this->interpolate(this->x);

		return (this->x < this->max_x);
	}

protected:
	virtual void interpolate (const Tx x) = 0;
};

// ---------------------------------------------------

template <typename Tx, typename Ty>
class Interpolator__ : public Interpolator<Tx>
{
protected:
	Ty& target;

public:
	Interpolator__ (const Tx max_x_, Ty *target_, const Ty start_y_)
		: Interpolator<Tx>(max_x_),
		target(*target_)
	{
		this->target = start_y_;
	}
};

// ---------------------------------------------------

template <typename Tx, typename Ty>
class LinearInterpolator : public Interpolator__<Tx, Ty>
{
protected:
	Ty start_y;
	Ty rate;

public:
	LinearInterpolator (const Tx max_x_, Ty *target_, const Ty start_y_, const Ty end_y_)
		: Interpolator__<Tx, Ty>(max_x_, target_, start_y_),
		  start_y(start_y_),
		  rate((end_y_ - start_y_) / max_x_)
	{
	}

protected:
	void interpolate (const Tx x) override final
	{
		this->target = this->start_y + x * this->rate;
	}
};

// ---------------------------------------------------

template <typename Tx>
class InterpolationManager
{
public:
	struct Event : public Mylib::Coroutine::Event {
		Interpolator<Tx> *interpolator;
	};

	struct Descriptor {
		Event *ptr;
	};

private:
	using InterpolatorCallback = Mylib::Trigger::Callback<Event>;

	struct EventCallback {
		InterpolatorCallback *callback;
	};

	struct EventCoroutine {
		CoroutineHandle coroutine_handler;
	};

	struct EventFull : public Event {
		std::variant<EventCallback, EventCoroutine> var_callback;
		bool enabled;
	};

	Memory::Manager& memory_manager;
	std::vector<EventFull*> interpolators;

public:
	InterpolationManager ()
		: memory_manager(Memory::default_manager)
	{
	}

	InterpolationManager (Memory::Manager& memory_manager_)
		: memory_manager(memory_manager_)
	{
	}

	template <typename Ty>
	Descriptor interpolate_linear (const Tx max_x_, Ty *target_, const Ty start_y_, const Ty end_y_, const InterpolatorCallback& callback)
	{
		EventFull *event = new (this->memory_manager.template allocate_type<EventFull>(1)) EventFull;
		event->interpolator = new (this->memory_manager.template allocate_type<LinearInterpolator<Tx, Ty>>(1)) LinearInterpolator<Tx, Ty>(max_x_, target_, start_y_, end_y_);
		event->var_callback = EventCallback {
			.callback = callback.make_copy(this->memory_manager),
		},
		event->enabled = true;

		this->push(event);
		
		return Descriptor { .ptr = event };
	}

private:
	inline void push (EventFull *event)
	{
		this->events.push_back(event);
	}
};

// ---------------------------------------------------

} // end namespace Mylib

#endif