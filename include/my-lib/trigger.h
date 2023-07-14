#ifndef __MY_LIBS_TIMER_HEADER_H__
#define __MY_LIBS_TIMER_HEADER_H__

#include <iostream>
#include <list>
#include <functional>

#include <cstdint>

#include <my-lib/macros.h>

namespace Mylib
{
namespace Trigger
{

// ---------------------------------------------------

template <typename T>
class Callback
{
public:
	virtual void operator() (const T& data) = 0;
};

// ---------------------------------------------------

/*
	Template parameter Tdata must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tdata&);
*/

template <typename Tdata, typename Tobj, typename Tfunc>
auto make_callback_object (Tobj& obj, Tfunc callback)
{
	class DerivedCallback : public Callback<Tdata>
	{
	private:
		Tobj& obj;
		Tfunc callback_function;

	public:
		DerivedCallback (Tobj& obj_, Tfunc callback_function_)
			: obj(obj_), callback_function(callback_function_)
		{
		}

		void operator() (const Tdata& data) override
		{
			auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj),
				std::forward_as_tuple(data)
			);
			
			std::apply(this->callback_function, built_params);
		}
	};

	return DerivedCallback(obj, callback);
}

// ---------------------------------------------------

/*
	Template parameter Tdata must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tdata&, parameters...);

	Important, the first parameter is the Event Data (Tdata).
*/

template <typename Tdata, typename Tobj, typename Tfunc, typename Tfirst_param, typename... Args>
auto make_callback_object_with_params (Tobj& obj, Tfunc callback, Tfirst_param first_param, Args&&... args)
{
	//auto params = std::make_tuple(first_param, std::forward<Args>(args)...);
	auto params = std::make_tuple(first_param, args...);

	using Tparams = decltype(params);

	class DerivedCallback : public Callback<Tdata>
	{
	private:
		Tobj& obj;
		Tfunc callback_function;
		Tparams callback_params;
	
	public:
		DerivedCallback (Tobj& obj_, Tfunc callback_function_, Tparams callback_params_)
			: obj(obj_), callback_function(callback_function_), callback_params(callback_params_)
		{
		}

		void operator() (const Tdata& data) override
		{
			auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj), // can use std::tie as well
				std::forward_as_tuple(data),
				this->callback_params
			);
			
			std::apply(this->callback_function, built_params);
			//std::invoke(this->callback_function, *(this->obj));
		}
	};

	return DerivedCallback(obj, callback, params);
}

// ---------------------------------------------------

template <typename T>
class Event
{
public:
	struct Subscriber {
		Callback<T>& callback;
		bool enabled;
	};

	struct Descriptor {
		Subscriber *subscriber;
	};

private:
	std::list<Subscriber> subscribers;

public:
	void publish (const T& event_data)
	{
		for (auto& subscriber : this->subscribers) {
			if (subscriber.enabled)
				subscriber.callback(event_data);
		}
	}

	Descriptor subscribe (Callback<T>& callback)
	{
		this->subscribers.push_back( Subscriber { .callback = callback, .enabled = true } );
		return Descriptor { .subscriber = &this->subscribers.back() };
	}

	void unsubscribe (Descriptor& descriptor)
	{
		this->subscribers.remove_if(
			[&descriptor] (Subscriber& subscriber) -> bool {
				return (descriptor.subscriber == &subscriber);
			}
		);
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif