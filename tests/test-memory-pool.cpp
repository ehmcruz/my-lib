#include <iostream>
#include <cassert>

#include <my-lib/memory-pool.h>

#define n 10000000
//#define n 10

void test_core ()
{
	Mylib::Memory::PoolCoreSameType<uint32_t> factory(1024);
	uint32_t i, correct;
	uint32_t **v, **p;

	v = (uint32_t**)malloc(n * sizeof(uint32_t*));
	assert(v != nullptr);

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.allocate();
		**p = i;
		p++;
	}

	correct = 0;
	for (i=0; i<n; i++) {
		if (*(v[i]) == i)
			correct++;
	}

	std::cout << correct << " elements are correct" << std::endl;

	p = v;
	for (i=0; i<n; i++) {
		factory.deallocate(*p);
		p++;
	}

	std::cout << "freed all elements" << std::endl;

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.allocate();
		**p = i;
		p++;
	}

	correct = 0;
	for (i=0; i<n; i++) {
		if (*(v[i]) == i)
			correct++;
	}

	std::cout << correct << " elements are correct again" << std::endl;
}

void test_general ()
{
	Mylib::Memory::PoolManager factory( { 10, 1, 14, 20, 9, 8 } );
	//datablock_alloc_t factory(34, 8);
//exit(1);
	uint32_t i, correct;
	uint32_t **v, **p;

	v = (uint32_t**)malloc(n * sizeof(uint32_t*));
	assert(v != nullptr);

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.allocate_type<uint32_t>(1);
		**p = i;
		p++;
	}

	correct = 0;
	for (i=0; i<n; i++) {
		if (*(v[i]) == i)
			correct++;
	}

	std::cout << correct << " elements are correct" << std::endl;

	p = v;
	for (i=0; i<n; i++) {
		factory.deallocate_type<uint32_t>(*p, 1);
		//factory.release(*p);
		//factory.release(*p, sizeof(uint32_t));
		p++;
	}

	std::cout << "freed all elements" << std::endl;

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.allocate_type<uint32_t>(1);
		//*p = factory.alloc();
		**p = i;
		p++;
	}

	correct = 0;
	for (i=0; i<n; i++) {
		if (*(v[i]) == i)
			correct++;
	}

	std::cout << correct << " elements are correct again" << std::endl;
}

int main ()
{
	test_core();
	std::cout << std::endl;
	test_general();

	return 0;
}