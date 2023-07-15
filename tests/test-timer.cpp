#include <iostream>
#include <vector>

#include <stdint.h>
#include <assert.h>

#include <my-lib/timer.h>

using Timer = Mylib::Trigger::Timer<uint32_t>;

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

Timer timer(0);

test_t test;

int main ()
{
	std::cout << "scheduling object function without params" << std::endl;
	timer.schedule_event(10, Mylib::Trigger::make_callback_object_with_params< Timer::Event >(test, &test_t::callback_1, 10));

	timer.schedule_event(20, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	std::cout << "created " << timer.get_n_scheduled_events() << " events" << std::endl;

	// ensure that test object was not copy/moved

	test.b = 99;

	timer.trigger_events(100);

	return 0;
}