#include <iostream>
#include <assert.h>

#include "data-factory.h"

#define n 100000000

int main ()
{
	data_factory_same_type_t<uint32_t> factory;
	uint32_t i, correct;
	uint32_t **v, **p;

	v = (uint32_t**)malloc(n * sizeof(uint32_t*));
	assert(v != nullptr);

	p = v;
	for (i=0; i<n; i++) {
		*p = factory.safe_alloc();
		**p = i;
		p++;
	}

	correct = 0;
	for (i=0; i<n; i++) {
		if (*(v[i]) == i)
			correct++;
	}

	std::cout << correct << " elements are correct" << std::endl;

	return 0;
}