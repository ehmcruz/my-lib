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
	bool bit;
	MyBitSet::Type bits;

	std::cout << "----------------------" << std::endl;
	bitset = 0x04;
	std::cout << bitset << std::endl;

	std::cout << "----------------------" << std::endl;
	bitset[0] = 1;
	std::cout << bitset << std::endl;

	std::cout << "----------------------" << std::endl;
	const MyBitSet cset(4);
	bit = cset[0];
	std::cout << bit << std::endl;

	std::cout << "----------------------" << std::endl;
	bit = bitset[0];
	std::cout << bit << std::endl;

	std::cout << "----------------------" << std::endl;
	bits = ~bitset(0, 2);
	std::cout << static_cast<uint64_t>(bits) << std::endl;

	std::cout << "----------------------" << std::endl;
	bitset(0, 2) = 0b0011;
	bitset[2] = 0;
	std::cout << bitset << std::endl;

	std::cout << "----------------------" << std::endl;
	bitset(0, 2) = bitset(2, 2);
	std::cout << bitset << std::endl;

	std::cout << "----------------------" << std::endl;
	bitset(1, 3) = cset(0, 3);
	std::cout << bitset << std::endl;

	std::cout << "----------------------" << std::endl;
	const Mylib::BitSet<3> bs (0b101);
	bitset(1, 3) = bs;
	std::cout << bitset << std::endl;

	return 0;
}