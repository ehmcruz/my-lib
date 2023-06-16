#include <iostream>
#include <list>
#include <deque>
#include <assert.h>

#include "datablock-alloc.h"

/*
	default allocator is in <include>/c++/11/ext/new_allocator.h

	check c++ default standard:
	g++ -dM -E -x c++  /dev/null | grep -F __cplusplus
*/

template <typename T>
class my_allocator_t
{
public:
	using size_type = std::size_t;
	using value_type = T;
//	using difference_type = std::ptrdiff_t;
/*
//#if __cplusplus <= 201703L
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	template<typename _T>
	struct rebind { typedef my_allocator_t<_T> other; };
//#endif
*/

	constexpr my_allocator_t () noexcept { }

	constexpr my_allocator_t (const my_allocator_t&) noexcept { }

	template<typename _T>
	constexpr my_allocator_t (const my_allocator_t<_T>&) noexcept { }

	[[nodiscard]] T* allocate (size_type n, const void* = static_cast<const void*>(0))
	{
		static_assert(sizeof(T) != 0, "cannot allocate incomplete types");

		std::cout << "allocating " << n << " elements of size " << sizeof(T) << std::endl;

	#if 0
		if (n != 1)
			std::__throw_bad_alloc();
	#endif

	#if __cpp_aligned_new
		if (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
			std::align_val_t align = std::align_val_t(alignof(T));
			return static_cast<T*>(::operator new(n * sizeof(T), align));
		}
	#endif

		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	void deallocate (T* p, size_type n)
	{
		std::cout << "deallocating " << n << " elements of size " << sizeof(T) << std::endl;

	#if __cpp_aligned_new
		if (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
			::operator delete(p,
		#if __cpp_sized_deallocation
			n * sizeof(T),
		#endif
			std::align_val_t(alignof(T)));
			return;
		}
	#endif
		::operator delete(p
		#if __cpp_sized_deallocation
		, n * sizeof(T)
		#endif
		);
	}
};

struct data_t_ {
	int v;
};

using data_t = int;

my_allocator_t<data_t> my_allocator;
std::list<data_t, my_allocator_t<data_t>> list;
std::deque<data_t, my_allocator_t<data_t>> list2;

int main ()
{
#if __cpp_aligned_new
	std::cout << "with aligned new" << std::endl;
	std::cout << "value of __STDCPP_DEFAULT_NEW_ALIGNMENT__ is " << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << std::endl;
#else
	std::cout << "without aligned new" << std::endl;
#endif

#if __cpp_sized_deallocation
	std::cout << "with sized deallocation" << std::endl;
#else
	std::cout << "without sized deallocation" << std::endl;
#endif

	std::cout << std::endl;

	list.push_back(5);
	list.push_back(10);

	for (auto v: list) {
		std::cout << "list value " << v << std::endl;
	}

	std::cout << std::endl;

	list2.push_back(5);
	list2.push_back(10);

	for (auto v: list2) {
		std::cout << "list2 value " << v << std::endl;
	}

	return 0;
}