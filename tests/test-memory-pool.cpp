#include <iostream>
#include <list>
#include <chrono>
#include <mutex>
#include <memory>
#include <utility>

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

void test_stl ()
{
	Mylib::Memory::PoolManager factory(1024, 8);
	Mylib::Memory::AllocatorSTL<uint32_t> stl_pool_allocator(factory);
	std::list<uint32_t, Mylib::Memory::AllocatorSTL<uint32_t>> list(stl_pool_allocator);

	auto start = std::chrono::system_clock::now();

	uint32_t i, correct;

	for (i=0; i<n; i++) {
		list.push_back(i);
	}

	correct = 0;
	i = 0;
	for (auto el : list) {
		if (el == i++)
			correct++;
	}

	//std::cout << correct << " elements are correct" << std::endl;

	list.clear();

	for (i=0; i<n; i++) {
		list.push_back(i);
	}

	correct = 0;
	i = 0;
	for (auto el : list) {
		if (el == i++)
			correct++;
	}

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << elapsed.count() << " miliseconds" << std::endl;

	std::cout << correct << " elements are correct again" << std::endl;
}

void benchmark_base_stl ()
{
	std::list<uint32_t> list;

	auto start = std::chrono::system_clock::now();

	uint32_t i, correct;

	for (i=0; i<n; i++) {
		list.push_back(i);
	}

	correct = 0;
	i = 0;
	for (auto el : list) {
		if (el == i++)
			correct++;
	}

	//std::cout << correct << " elements are correct" << std::endl;

	list.clear();

	for (i=0; i<n; i++) {
		list.push_back(i);
	}

	correct = 0;
	i = 0;
	for (auto el : list) {
		if (el == i++)
			correct++;
	}

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << elapsed.count() << " miliseconds" << std::endl;

	std::cout << correct << " elements are correct again" << std::endl;
}

struct xxx {
	int a;
	int b;
};

struct xxx_derived : public xxx {
	int c;
	int d;
};

void test_unique_ptr ()
{
	auto& default_manager = Mylib::Memory::default_manager;
	auto& default_allocator_stl = Mylib::Memory::default_allocator_stl;

	Mylib::Memory::DeAllocatorSTL<int> deallocator(default_manager);
	Mylib::Memory::DeAllocatorSTL<int> deallocator2(default_allocator_stl);

	Mylib::Memory::unique_ptr<int> ptr(default_manager.allocate_type<int>(1), deallocator);
	std::cout << "ptr " << ptr.get() << std::endl;
}

void test_unique_ptr_derived ()
{
	auto& default_manager = Mylib::Memory::default_manager;
	auto& default_allocator_stl = Mylib::Memory::default_allocator_stl;

	Mylib::Memory::unique_ptr<xxx_derived> ptr = Mylib::Memory::make_unique<xxx_derived>(default_manager);
	//std::unique_ptr<xxx_derived> ptr(new xxx_derived);

	Mylib::Memory::unique_ptr<xxx> ptr2(std::move(ptr));

	std::cout << "ptr2 " << ptr2.get() << std::endl;
}

void test_shared_ptr ()
{
	auto& default_manager = Mylib::Memory::default_manager;
	Mylib::Memory::AllocatorSTL<int> allocator(default_manager);

	std::shared_ptr<int> ptr;
	ptr = std::allocate_shared<int>(allocator, 1);
	std::cout << "ptr " << ptr.get() << std::endl;
}

int main ()
{
	std::cout << "---------------------------------- unique_ptr start" << std::endl;
	test_unique_ptr();
	std::cout << "---------------------------------- unique_ptr end" << std::endl;

	std::cout << "---------------------------------- unique_ptr derived start" << std::endl;
	test_unique_ptr_derived();
	std::cout << "---------------------------------- unique_ptr derived end" << std::endl;

	std::cout << "---------------------------------- shared_ptr start" << std::endl;
	test_shared_ptr();
	std::cout << "---------------------------------- shared_ptr end" << std::endl;
	return 0;

	std::cout << "----------------------------------" << std::endl;
	test_core();

	std::cout << "----------------------------------" << std::endl;
	test_general();
	
	std::cout << "---------------------------------- memory pool STL" << std::endl;
	test_stl();

	std::cout << "---------------------------------- base STL" << std::endl;
	benchmark_base_stl();

	return 0;
}