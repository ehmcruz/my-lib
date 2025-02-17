#ifndef __MY_LIB_COROUTINE_HEADER_H__
#define __MY_LIB_COROUTINE_HEADER_H__

#include <coroutine>

#include <my-lib/std.h>


namespace Mylib
{

// ---------------------------------------------------

struct Coroutine {
	// Used so that we can control the coroutine in timer and inperpolator.
	struct Event { };

	struct promise_type {
		// If the coroutine is not waiting for a timer event, this is nullptr.
		// Otherwise, it points to the event.
		// We need this to be able to destroy the event when the coroutine is unregistered.
		Event *event;

		Coroutine get_return_object ()
		{
			return Coroutine {
				.handler = std::coroutine_handle<promise_type>::from_promise(*this)
			};
		}

		// Suspend the coroutine immediately after creation.
		std::suspend_always initial_suspend () const noexcept { return {}; }

		// https://stackoverflow.com/questions/75778999/should-promises-final-suspend-always-return-stdsuspend-always
		// Quoting:
		// It would be better to say that final_suspend should "always suspend",
		// rather than "always return std::suspend_always".
		// A coroutine is considered to be done if it is suspended at its
		// final-suspend point. So if a coroutine flows past this point without
		// suspending, then the coroutine is over, but coroutine_handle::done() will not be true.
		//
		// In summary, in order to be able to check if a coroutine is done, using
		// coroutine_handle::done(), the final_suspend should always suspend.
		std::suspend_always final_suspend () const noexcept { return {}; }

		// co_return returns nothing.
		void return_void () const noexcept {}

		void unhandled_exception () { std::terminate(); }
	};

	std::coroutine_handle<promise_type> handler;
};

using PromiseType = typename Coroutine::promise_type;
using CoroutineHandle = std::coroutine_handle<PromiseType>;

// ---------------------------------------------------

} // end namespace Mylib

#endif