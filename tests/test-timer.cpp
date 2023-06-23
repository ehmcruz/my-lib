#include <iostream>
#include <vector>

#include <stdint.h>
#include <assert.h>

#include <my-lib/timer.h>

Mylib::timer_t<uint32_t> timer;

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

	void callback ()
	{
		std::cout << "oba " << b << std::endl;
	}

	void callback_1 (int n)
	{
		std::cout << "uhu " << b << " " << n << std::endl;
	}

	void callback_2 (int n, float c)
	{
		std::cout << "ueba " << b << " " << n << " - " << c << std::endl;
	}
};

test_t test;

int main ()
{
	std::cout << "scheduling object function without params" << std::endl;
	timer.schedule_event(10, &test, &test_t::callback);

	std::cout << "scheduling object function with 1 param" << std::endl;
	timer.schedule_event_object(10, &test, &test_t::callback_1, 10);

	std::cout << "scheduling object function with 2 params" << std::endl;
	timer.schedule_event_object(10, &test, &test_t::callback_2, 5, 3.2);

	std::cout << "created " << timer.get_n_scheduled_events() << " events" << std::endl;

	// ensure that test object was not copy/moved

	test.b = 99;

	timer.trigger_events(1);

	return 0;
}