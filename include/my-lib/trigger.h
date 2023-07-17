#ifndef __MY_LIBS_TRIGGER_HEADER_H__
#define __MY_LIBS_TRIGGER_HEADER_H__

#include <iostream>
#include <list>
#include <functional>
#include <type_traits>

#include <cstdint>

#include <my-lib/macros.h>
#include <my-lib/std.h>

namespace Mylib
{
namespace Trigger
{

// ---------------------------------------------------

template <typename Tevent>
class Callback
{
public:
	virtual void operator() (Tevent& event) = 0;
	virtual ~Callback () = default;
};

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&);
*/

template <typename Tevent, typename Tobj, typename Tfunc>
auto make_callback_object (Tobj& obj, Tfunc callback)
{
	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tobj& obj;
		Tfunc callback_function;

	public:
		DerivedCallback (Tobj& obj_, Tfunc callback_function_)
			: obj(obj_), callback_function(callback_function_)
		{
		}

		void operator() (Tevent& event) override
		{
			/*auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj),
				std::forward_as_tuple(event)
			);
			std::apply(this->callback_function, built_params);*/
			std::invoke(this->callback_function, this->obj, event);
		}
	};

	return DerivedCallback(obj, callback);
}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&, const Tfilter&);
*/

template <typename Tevent, typename Tfilter_, typename Tobj, typename Tfunc>
auto make_filter_callback_object (Tfilter_&& filter, Tobj& obj, Tfunc callback)
{
	using Tfilter = remove_type_qualifiers<Tfilter_>::type;

	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tfilter filter;
		Tobj& obj;
		Tfunc callback_function;

	public:
		DerivedCallback (Tfilter& filter_, Tobj& obj_, Tfunc callback_function_)
			: filter(filter_), obj(obj_), callback_function(callback_function_)
		{
		}

		void operator() (Tevent& event) override
		{
			if (this->filter(event)) {
				/*auto built_params = std::tuple_cat(
					std::forward_as_tuple(this->obj),
					std::forward_as_tuple(event),
					std::forward_as_tuple(this->filter)
				);
				std::apply(this->callback_function, built_params);*/
				std::invoke(this->callback_function, this->obj, event, this->filter);
			}
		}
	};

	return DerivedCallback(filter, obj, callback);
}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&, parameters...);

	Important, the first parameter is the Event Data (Tevent).
*/

template <typename Tevent, typename Tobj, typename Tfunc, typename Tfirst_param, typename... Args>
auto make_callback_object_with_params (Tobj& obj, Tfunc callback, const Tfirst_param& first_param, Args&&... args)
{
	//auto params = std::make_tuple(first_param, std::forward<Args>(args)...);
	auto params = std::make_tuple(first_param, args...);

	using Tparams = decltype(params);

	class DerivedCallback : public Callback<Tevent>
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

		void operator() (Tevent& event) override
		{
			auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj), // can use std::tie as well
				std::forward_as_tuple(event),
				this->callback_params
			);
			
			std::apply(this->callback_function, built_params);
			//std::invoke(this->callback_function, *(this->obj));
		}
	};

	return DerivedCallback(obj, callback, params);
}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&, const Tfilter&, parameters...);

	Important, the first parameter is the Event Data (Tevent).
	Important, the second parameter is the Filter Data (Tfilter).
*/

template <typename Tevent, typename Tfilter_, typename Tobj, typename Tfunc, typename Tfirst_param, typename... Args>
auto make_filter_callback_object_with_params (Tfilter_&& filter, Tobj& obj, Tfunc callback, const Tfirst_param& first_param, Args&&... args)
{
	using Tfilter = remove_type_qualifiers<Tfilter_>::type;

	//auto params = std::make_tuple(first_param, std::forward<Args>(args)...);
	auto params = std::make_tuple(first_param, args...);

	using Tparams = decltype(params);

	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tfilter filter;
		Tobj& obj;
		Tfunc callback_function;
		Tparams callback_params;
	
	public:
		DerivedCallback (Tfilter& filter_, Tobj& obj_, Tfunc callback_function_, Tparams callback_params_)
			: filter(filter_), obj(obj_), callback_function(callback_function_), callback_params(callback_params_)
		{
		}

		void operator() (Tevent& event) override
		{
			if (this->filter(event)) {
				auto built_params = std::tuple_cat(
					std::forward_as_tuple(this->obj), // can use std::tie as well
					std::forward_as_tuple(event),
					std::forward_as_tuple(this->filter),
					this->callback_params
				);
				
				std::apply(this->callback_function, built_params);
				//std::invoke(this->callback_function, *(this->obj));
			}
		}
	};

	return DerivedCallback(filter, obj, callback, params);
}

// ---------------------------------------------------

template <typename Tevent>
class EventHandler
{
public:
	using Type = Tevent;

	struct Subscriber {
		Callback<Tevent> *callback;
		bool enabled;
	};

	struct Descriptor {
		Subscriber *subscriber;
	};

private:
	std::list<Subscriber> subscribers;

public:
	~EventHandler ()
	{
		for (auto& subscriber : this->subscribers)
			delete subscriber.callback;
	}

	// We don't use const Tevent& because we allow the user to manipulate event data.
	// This is useful for the timer, allowing us to re-schedule events.
	void publish (Tevent& event)
	{
		for (auto& subscriber : this->subscribers) {
			if (subscriber.enabled) {
				auto& c = *(subscriber.callback);
				c(event);
			}
		}
	}

	void publish (Tevent&& event)
	{
		this->publish(event);
	}

	/* When creating the event listener by r-value ref,
	   we allocate internal storage and copy the value to it.
	*/
	template <typename Tcallback>
	Descriptor subscribe (const Tcallback& callback)
		//requires std::is_rvalue_reference<decltype(callback)>::value
	{
		//std::cout << "here R-VALUE " << callback.filter.myself->get_name() << std::endl;
		using Tc = Mylib::remove_type_qualifiers< decltype(callback) >::type;

		//Tc *persistent_callback = new Tc( static_cast<Tc&>(callback) );
		Tc *persistent_callback = new Tc(callback);
		//std::cout << "persistent " << persistent_callback->filter.myself->get_name() << std::endl;
		this->subscribers.push_back( Subscriber { .callback = persistent_callback, .enabled = true } );
		//std::cout << "LIST " << static_cast<Tc*>(this->subscribers.back().callback)->filter.myself->get_name() << std::endl;
		return Descriptor { .subscriber = &this->subscribers.back() };
	}

	void unsubscribe (Descriptor& descriptor)
	{
		this->subscribers.remove_if(
			[&descriptor] (Subscriber& subscriber) -> bool {
				bool found = (descriptor.subscriber == &subscriber);

				if (found)
					delete subscriber.callback;
				
				return found;
			}
		);
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif