#ifndef __MY_LIBS_TIMER_HEADER_H__
#define __MY_LIBS_TIMER_HEADER_H__

#include <iostream>
#include <vector>
#include <queue>
#include <functional>

#include <stdint.h>
#include <stdlib.h>

#include <my-lib/macros.h>

namespace Mylib
{

// ---------------------------------------------------

template <typename Ttime>
class timer_t
{
public:
	class event_t
	{
		OO_ENCAPSULATE_READONLY(Ttime, time)

	public:
		event_t (Ttime time_)
			: time(time_)
		{
		}

		virtual void callback () = 0;
	};

private:
	std::priority_queue< event_t* > events;
	Ttime current_time;

public:
	timer_t (Ttime initial_time = 0)
		: current_time(initial_time)
	{

	}

	inline uint32_t get_n_scheduled_events ()
	{
		return this->events.size();
	}

	void trigger_events (Ttime time)
	{
		event_t *event;

		this->current_time = time;

		while (!this->events.empty()) {
			event = this->events.top();

			if (event->get_time() >= time) {
				event->callback();
				this->events.pop();

				delete event;
			}
			else
				break;
		}
	}

	void schedule_event (event_t *event)
	{
		this->events.push(event);
	}

	template <typename Tobj, typename Tfunc>
	void schedule_event (Ttime time, Tobj *obj, Tfunc callback)
	{
		class derived_event_t: public event_t
		{
		public:
			Tobj *obj;
			Tfunc callback_function;
		
			using event_t::event_t;

			void callback () override
			{
				std::cout << "test" << std::endl;
				std::invoke(this->callback_function, *(this->obj));
			}
		};

		derived_event_t *event = new derived_event_t(time);
		event->obj = obj;
		event->callback_function = callback;

		this->schedule_event(event);
	}

#if 0
	template <typename Tobj, typename Tfunc>
	void schedule_event (Ttime time, Tobj *obj, Tfunc callback)
	{
		class derived_event_t: public event_t
		{
		public:
			Tobj *obj;
			Tfunc callback_function;
		
			using event_t::event_t;

			void callback () override
			{
				std::cout << "test" << std::endl;
				std::invoke(this->callback_function, *(this->obj));
			}
		};

		derived_event_t *event = new derived_event_t(time);
		event->obj = obj;
		event->callback_function = callback;

		this->schedule_event(event);
	}
#endif

	/*
		Schedule an event that calls a function of an object with n-parameters.
		Honestly, I have my doubts if this would be possible.
		Thankfully, a crazy C++ mix of templates, auto e decltype made it possible.
	*/

	template <typename Tobj, typename Tfunc, typename Tfirst_param, typename... Args>
	void schedule_event_object (Ttime time, Tobj *obj, Tfunc callback, Tfirst_param first_param, Args&&... args)
	{
		auto params = std::tuple_cat( std::forward_as_tuple(*obj), // can use std::tie as well
		                              std::make_tuple(first_param, std::forward<Args>(args)...)
									);

		this->schedule_event_with_params(time, callback, params);
	}

private:
	template <typename Tfunc, typename Tparams>
	void schedule_event_with_params (Ttime time, Tfunc callback, Tparams& params)
	{
		class derived_event_t: public event_t
		{
		public:
			Tfunc callback_function;
			Tparams callback_params;
		
			derived_event_t (Ttime time, Tfunc callback_function_, Tparams callback_params_)
				: event_t(time), callback_function(callback_function_), callback_params(callback_params_)
			{
			}

			void callback () override
			{
				std::cout << "test" << std::endl;
				std::apply(this->callback_function, this->callback_params);
				//std::invoke(this->callback_function, *(this->obj));
			}
		};

		derived_event_t *event = new derived_event_t(time, callback, params);
		this->schedule_event(event);
	}

#if 0
	template <typename Tobj, typename Tfunc, typename Tfirst_param, typename... Args>
	void schedule_event_object (Ttime time, Tobj *obj, Tfunc callback, Tfirst_param first_param, Args&&... args)
	{
		auto params = std::make_tuple( first_param, std::forward<Args>(args)... );

		class derived_event_t: public event_t
		{
		public:
			Tobj *obj;
			Tfunc callback_function;
			decltype(params) callback_params;
		
			using event_t::event_t;

			void callback () override
			{
				Tobj& o = *(this->obj);

				auto params_with_obj = std::tuple_cat( std::forward_as_tuple(o), this->callback_params );
				std::cout << "test" << std::endl;
				std::apply(this->callback_function, params_with_obj);
				//std::invoke(this->callback_function, *(this->obj));
			}
		};

		derived_event_t *event = new derived_event_t(time);
		event->obj = obj;
		event->callback_function = callback;
		event->callback_params = params;

		this->schedule_event(event);
	}
#endif
};

// ---------------------------------------------------

} // end namespace

#endif