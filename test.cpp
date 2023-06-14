#include <iostream>
#include <assert.h>

#include "datablock-alloc.h"

#define n 10000000
//#define n 10

void test_core ()
{
	datablock_alloc_same_type_t<uint32_t> factory;
	uint32_t i, correct;
	uint32_t **v, **p;

	v = (uint32_t**)malloc(n * sizeof(uint32_t*));
	assert(v != nullptr);

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.alloc();
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
		factory.release(*p);
		p++;
	}

	std::cout << "freed all elements" << std::endl;

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.alloc();
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
	datablock_alloc_t factory( { 10, 1, 14, 20, 9, 8 } );
	//datablock_alloc_t factory(34, 8);
//exit(1);
	uint32_t i, correct;
	uint32_t **v, **p;

	v = (uint32_t**)malloc(n * sizeof(uint32_t*));
	assert(v != nullptr);

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.alloc<uint32_t>();
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
		factory.release<uint32_t>(*p);
		//factory.release(*p);
		//factory.release(*p, sizeof(uint32_t));
		p++;
	}

	std::cout << "freed all elements" << std::endl;

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.alloc<uint32_t>();
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