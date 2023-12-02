#include <iostream>
#include <vector>
#include <coroutine>
#include <list>

#include <cstdint>
#include <cassert>

#include <my-lib/timer.h>

uint32_t global_time = 0;

uint32_t get_time ()
{
	return global_time;
}

//using Timer = Mylib::Trigger::Timer<uint32_t, decltype(get_time)>;
//auto timer = Mylib::Trigger::Timer(get_time);

Mylib::Trigger::Timer timer(get_time);

using Timer = decltype(timer);

struct MyTestTimer {
	uint32_t get_current_time ()
	{
		return global_time;
	}

	struct Coroutine {
		struct promise_type {
			Coroutine get_return_object ()
			{
				return Coroutine {
					.handler = std::coroutine_handle<promise_type>::from_promise(*this)
				};
			}

			std::suspend_always initial_suspend () const noexcept { return {}; }
			std::suspend_always final_suspend () const noexcept { return {}; }
			void return_void() const noexcept {}
			void unhandled_exception () { std::terminate(); }
		};

		struct Awaiter {
			MyTestTimer& timer;
			const uint32_t seconds;

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

			void await_suspend (std::coroutine_handle<promise_type> handler)
			{
				auto& promise = handler.promise();
				Coroutine coro = promise.get_return_object();

				Event event = {
					.time = this->timer.get_current_time() + this->seconds,
					.coro = coro
				};

				std::cout << "\tawait_suspend event time=" << event.time << std::endl;
				this->timer.events.push_back(event);
			}

			// await_resume is called when the coroutine is resumed.
			// Return void since we don't have a value to return to the caller.

			constexpr void await_resume () const noexcept { }
		};

		std::coroutine_handle<promise_type> handler;
	};

	struct Event {
		uint32_t time;
		Coroutine coro;
	};

	std::list<Event> events; // we will use a heap in final version

	void register_coroutine (Coroutine coro)
	{
		// We created the coroutine in a suspended state.
		// We need to resume it to start the execution.
		coro.handler.resume();
	}

	Coroutine::Awaiter coroutine_wait (uint32_t seconds)
	{
		return Coroutine::Awaiter {
			.timer = *this,
			.seconds = seconds
		};
	}

	void process_events ()
	{
		for (auto it = this->events.begin(); it != this->events.end(); ) {
			Event& event = *it;

			if (event.time <= this->get_current_time()) {
				std::cout << "processing event time=" << event.time << std::endl;
				event.coro.handler.resume();

				it = this->events.erase(it);
			}
			else
				it++;
		}
	}

	void clear_events ()
	{
		this->events.clear();
	}
};

MyTestTimer test_timer;
bool alive = true;

MyTestTimer::Coroutine coro_print_values ()
{
	while (alive) {
		std::cout << "\tcoro_print_values loop start time=" << global_time << std::endl;
		co_await test_timer.coroutine_wait(3);
		std::cout << "\tcoro_print_values loop end time=" << global_time << std::endl;
	}
}

void test_coroutine ()
{
	std::cout << "test_coroutine" << std::endl;

	global_time = 0;
return;
	MyTestTimer::Coroutine timer_coroutine = coro_print_values();

	test_timer.register_coroutine(timer_coroutine);

	while (alive) {
		std::cout << "process events time=" << global_time << std::endl;
		test_timer.process_events();

		global_time++;

		if (global_time > 10)
			alive = false;
	}

	test_timer.clear_events();
}

class test_t
{
public:

	int b;

	test_t ()
	{
		this->b = 5555555;
		std::cout << "test constructor" << std::endl;
	}

	test_t (const test_t& other)
	{
		this->b = other.b;
		std::cout << "test COPY constructor" << std::endl;
	}

	test_t (const test_t&& other)
	{
		this->b = other.b;
		std::cout << "test MOVE constructor" << std::endl;
	}

	void callback (Timer::Event& event)
	{
		std::cout << "oba " << b  << " time " << event.time << std::endl;
	}

	void callback_1 (const Timer::Event& event, int n)
	{
		std::cout << "uhu1 " << b << " time " << event.time << " n " << n << std::endl;
	}

	void callback_2 (int n, float c)
	{
		std::cout << "uhu2 " << b << " " << n << " - " << c << std::endl;
	}
};

test_t test;

int main ()
{
	std::cout << "scheduling object function without params" << std::endl;
	timer.schedule_event(10, Mylib::Trigger::make_callback_object_with_params< Timer::Event >(test, &test_t::callback_1, 10));

	timer.schedule_event(20, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	timer.schedule_event(5, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	timer.schedule_event(1, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	timer.schedule_event(3, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	auto d = timer.schedule_event(55, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	std::cout << "created " << timer.get_n_scheduled_events() << " events" << std::endl;

	timer.unschedule_event(d);

	std::cout << "created " << timer.get_n_scheduled_events() << " events" << std::endl;

	// ensure that test object was not copy/moved

	test.b = 99;

	global_time = 100;

	timer.trigger_events();

	test_coroutine();

	return 0;
}