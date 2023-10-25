#include <iostream>
#include <vector>
#include <string>

#include <cstdint>
#include <cassert>

#include <my-lib/bit.h>

int main ()
{
	using MyBitSet = Mylib::BitSet<8>;

	MyBitSet bitset;

	bitset = 0x04;

	std::cout << bitset;

	std::cout << std::endl;

	return 0;
}