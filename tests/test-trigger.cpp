#include <iostream>
#include <vector>

#include <cstdint>
#include <cassert>

#include <my-lib/trigger.h>



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

	void callback_1 (const int& event_data, int n)
	{
		std::cout << "uhu1 " << b << " event_data " << event_data << " n " << n << std::endl;
	}

	void callback_2 (int n, float c)
	{
		std::cout << "uhu2 " << b << " " << n << " - " << c << std::endl;
	}

	void callback_3 (const int& event_data)
	{
		std::cout << "uhu3 " << b << " event_data " << event_data << std::endl;
	}
};

test_t test;

int main ()
{
	auto callback1 = Mylib::Trigger::make_callback_object_with_params<int>(test, &test_t::callback_1, 10);

	auto callback3 = Mylib::Trigger::make_callback_object<int>(test, &test_t::callback_3);

	Mylib::Trigger::Event<int> event;

	auto d1 = event.subscribe(callback1);
	auto d3 = event.subscribe(callback3);

	test.b = 99;

	event.publish(50);

	event.unsubscribe(d1);

	event.publish(67);

	return 0;
}