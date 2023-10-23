#include <iostream>
#include <vector>
#include <string>

#include <cstdint>
#include <cassert>

#include <my-lib/any.h>

int main ()
{
	Mylib::Any<801> data, data2;

	std::cout << "sizeof data is " << sizeof(data) << std::endl;

	data = 26;
	std::cout << "any has value " << data.get_value<int32_t>() << std::endl;

	std::string name = "John";
	data = name;
	std::cout << "any has value " << data.get_value<std::string>() << std::endl;

	data2 = data;

	std::cout << "any2 has value " << data2.get_value<std::string>() << std::endl;

	Mylib::Any<801> data3(data2);

	std::cout << "any3 has value " << data3.get_value<std::string>() << std::endl;

	return 0;
}