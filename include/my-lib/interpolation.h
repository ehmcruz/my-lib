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

	virtual void* get_target () const noexcept = 0; // returns target object

	virtual std::size_t get_size () const noexcept = 0; // returns object size
	virtual uint32_t get_alignment () const noexcept = 0; // returns object alignment
	virtual void deconstruct_free_memory (Memory::Manager& memory_manager) = 0;

protected:
	virtual void interpolate (const Tx x) = 0;
};

// ---------------------------------------------------

#define MYLIB_INTERPOLATOR_BASE_FUNCTIONS(CLASS) \
	public: \
		std::size_t get_size () const noexcept override final \
		{ \
			return sizeof(*this); \
		} \
		uint32_t get_alignment () const noexcept override final \
		{ \
			return alignof(decltype(*this)); \
		} \
		void deconstruct_free_memory (Memory::Manager& memory_manager) override final \
		{ \
			this->~CLASS(); \
			memory_manager.deallocate(this, this->get_size(), 1, this->get_alignment()); \
		} \
		static void* allocate (Memory::Manager& memory_manager) \
		{ \
			return memory_manager.allocate(sizeof(CLASS), 1, alignof(CLASS)); \
		}

// ---------------------------------------------------

template <typename Tx, typename Ty>
class Interpolator__ : public Interpolator<Tx>
{
protected:
	Ty& target;

public:
	Interpolator__ (const Tx max_x_, Ty *target_, const Ty& start_y_)
		: Interpolator<Tx>(max_x_),
		target(*target_)
	{
		this->target = start_y_;
	}

	void* get_target () const noexcept override final
	{
		return &this->target;
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
	LinearInterpolator (const Tx max_x_, Ty *target_, const Ty& start_y_, const Ty& end_y_)
		: Interpolator__<Tx, Ty>(max_x_, target_, start_y_),
		  start_y(start_y_),
		  rate((end_y_ - start_y_) / max_x_)
	{
	}

	MYLIB_INTERPOLATOR_BASE_FUNCTIONS(LinearInterpolator)

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
	struct Event {
		Interpolator<Tx> *interpolator;
	};

	struct Descriptor {
		Event *ptr;
	};

	struct CoroutineAwaiter {
		InterpolationManager& interpolation_manager;
		Interpolator<Tx> *interpolator;
		CoroutineHandle handler;

		// await_ready is called before the coroutine is suspended.
		// We return false to tell the caller to suspend.

		constexpr bool await_ready () const noexcept
		{
			return false;
		}

		// await_suspend is called when the coroutine is suspended.
		// Return void to tell the caller to suspend.
		// Only at this point we have access to the coroutine handler so we can
		// resume the coroutine later, when the timer expires.

		void await_suspend (CoroutineHandle handler)
		{
			this->handler = handler;
			PromiseType& promise = handler.promise();
			//Coroutine coro = promise.get_return_object();

			EventFull *event = new (this->interpolation_manager.memory_manager.template allocate_type<EventFull>(1)) EventFull;
			event->interpolator = this->interpolator;
			event->var_callback = EventCoroutine {
				.coroutine_handler = handler
			},

			// Store the event in the coroutine promise.
			// We do this to be able to destroy the event when the coroutine is unregistered.
			promise.awaiter_owner = &this->interpolation_manager;
			promise.awaiter_data = event;

			this->interpolation_manager.push(event);
		}

		// await_resume is called when the coroutine is resumed.
		// Return void since we don't have a value to return to the caller.

		constexpr void await_resume () const noexcept
		{
			PromiseType& promise = this->handler.promise();
			promise.awaiter_owner = nullptr;
			promise.awaiter_data = nullptr;
			//std::cout << "\t\tawait_resume" << std::endl;
		}
	};

	friend class CoroutineAwaiter;

private:
	using InterpolatorCallback = Mylib::Trigger::Callback<Event>;

	struct EventCallback {
		InterpolatorCallback *callback;
	};

	struct EventCoroutine {
		CoroutineHandle coroutine_handler;
	};

	struct EventFull : public Event {
		std::size_t vector_pos;
		std::variant<EventCallback, EventCoroutine> var_callback;
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

	~InterpolationManager ()
	{
		for (EventFull *event : this->interpolators)
			this->destroy_event(event);
	}

	void process_interpolation (const Tx delta_x)
	{
		for (std::size_t i = 0; i < this->interpolators.size(); i++) {
			EventFull *event = this->interpolators[i];

			if (!(*event->interpolator)(delta_x)) { // interpolation finished
				if (std::holds_alternative<EventCallback>(event->var_callback)) {
					EventCallback& callback = std::get<EventCallback>(event->var_callback);

					if (callback.callback) {
						auto& c = *(callback.callback);
						c(*event);
					}
				}
				else if (std::holds_alternative<EventCoroutine>(event->var_callback)) {
					EventCoroutine& event_coro = std::get<EventCoroutine>(event->var_callback);
					event_coro.coroutine_handler.resume(); // resume automatically sets promise.event to nullptr
				}

				this->pop(event);
				this->destroy_event(event);
			}
		}
	}

	template <typename Ty>
	Descriptor interpolate_linear (const Tx max_x_, Ty *target_, const Ty start_y_, const Ty end_y_)
	{
		EventFull *event = new (this->memory_manager.template allocate_type<EventFull>(1)) EventFull;
		event->interpolator = new (LinearInterpolator<Tx, Ty>::allocate(this->memory_manager)) LinearInterpolator<Tx, Ty>(max_x_, target_, start_y_, end_y_);
		event->var_callback = EventCallback {
			.callback = nullptr,
		},

		this->push(event);
		
		return Descriptor { .ptr = event };
	}

	template <typename Ty>
	Descriptor interpolate_linear (const Tx max_x_, Ty *target_, const Ty start_y_, const Ty end_y_, const InterpolatorCallback& callback)
	{
		EventFull *event = new (this->memory_manager.template allocate_type<EventFull>(1)) EventFull;
		event->interpolator = new (LinearInterpolator<Tx, Ty>::allocate(this->memory_manager)) LinearInterpolator<Tx, Ty>(max_x_, target_, start_y_, end_y_);
		event->var_callback = EventCallback {
			.callback = callback.make_copy(this->memory_manager),
		},

		this->push(event);
		
		return Descriptor { .ptr = event };
	}

	template <typename Ty>
	CoroutineAwaiter coroutine_wait_interpolate_linear (const Tx max_x_, Ty *target_, const Ty start_y_, const Ty end_y_)
	{
		return CoroutineAwaiter {
			.interpolation_manager = *this,
			.interpolator = new (LinearInterpolator<Tx, Ty>::allocate(this->memory_manager)) LinearInterpolator<Tx, Ty>(max_x_, target_, start_y_, end_y_)
		};
	}

	inline void remove_interpolator (Descriptor& descriptor)
	{
		EventFull *event = static_cast<EventFull*>(descriptor.ptr);
		this->pop(event);
		this->destroy_event(event);
	}

	inline void force_resume_coroutine (Coroutine coro)
	{
		PromiseType& promise = coro.handler.promise();

		if (promise.awaiter_owner.get_value<void*>() == static_cast<void*>(this)) {
			EventFull *event = promise.awaiter_data.get_value<EventFull*>();
			coro.handler.resume(); // resume automatically sets promise owner to nullptr
			this->pop(event);
			this->destroy_event(event);
		}
	}

	inline void unregister_coroutine (Coroutine coro)
	{
		PromiseType& promise = coro.handler.promise();

		if (promise.awaiter_owner.get_value<void*>() == static_cast<void*>(this)) {
			EventFull *event = promise.awaiter_data.get_value<EventFull*>();
			this->pop(event);
			this->destroy_event(event);
			promise.awaiter_owner = nullptr;
			promise.awaiter_data = nullptr;
		}
	}

private:
	inline void destroy_event (EventFull *event)
	{
		if (std::holds_alternative<EventCallback>(event->var_callback)) {
			EventCallback& callback = std::get<EventCallback>(event->var_callback);

			if (callback.callback)
				callback.callback->deconstruct_free_memory(this->memory_manager);
		}
		event->interpolator->deconstruct_free_memory(this->memory_manager);
		this->memory_manager.template deallocate_type<EventFull>(event, 1);
	}

	inline void push (EventFull *event)
	{
		event->vector_pos = this->interpolators.size();
		this->interpolators.push_back(event);
	}

	inline void pop (const std::size_t i)
	{
		const auto size = this->interpolators.size();

		if (size > 1) {
			this->interpolators[i] = this->interpolators[size - 1];
			this->interpolators[i]->vector_pos = i; // update vector_pos
		}

		this->interpolators.pop_back();
	}

	inline void pop (EventFull *event)
	{
		this->pop(event->vector_pos);
	}
};

// ---------------------------------------------------

#undef MYLIB_INTERPOLATOR_BASE_FUNCTIONS

// ---------------------------------------------------

} // end namespace Mylib

#endif