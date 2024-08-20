#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <cstdint>
#include <cassert>

#include <my-lib/bit.h>

struct BitField
{
	using Type = uint8_t;

	Type a : 2;
	Type b : 2;
	Type c : 4;
};

void test_bit_field ()
{
	using MyBitSet = Mylib::BitSetWrapper<BitField>;

	MyBitSet bitset;

	std::cout << "------------------------------------" << std::endl;
	std::cout << "Bit field test" << std::endl << std::endl;

	bitset = 0;
	std::cout << bitset << std::endl;

	bitset.b = 1;
	std::cout << bitset << std::endl;

	bitset.c = 1;
	std::cout << bitset << std::endl;
}

void test_enum ()
{
	using MyBitSet = Mylib::BitSet<16>;

	enum class menumc : uint8_t
	{
		a = 0,
		b = 1,
		c = 2,
		d = 3
	};

	enum menum
	{
		xa = 0,
		xb = 1,
		xc = 2,
		xd = 3
	};

	std::cout << "------------------------------------" << std::endl;
	std::cout << "Test ENUM" << std::endl << std::endl;

	MyBitSet bitset;

	std::cout << bitset << std::endl;

	bitset[xa] = 1;
	std::cout << bitset << std::endl;

	bitset[menumc::b] = 1;
	std::cout << bitset << std::endl;

	bitset(menumc::d, menumc::c) = 3;
	std::cout << bitset << std::endl;

	const MyBitSet cset(bitset);
	std::cout << "bit xa: " << cset[xa] << std::endl;
	std::cout << "bit b: " << cset[menumc::b] << std::endl;
	std::cout << "bit [d, b]:" << cset(menumc::d, menumc::c) << std::endl;
}

int main ()
{
	using MyBitSet = Mylib::BitSet<16>;

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

	std::stringstream ss;
	std::cout << "----------------------" << std::endl;
	ss << bitset;
	std::cout << ss.str() << std::endl;

	test_bit_field();
	test_enum();

	return 0;
}