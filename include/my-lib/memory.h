#ifndef __MY_LIB_MEMORY_HEADER_H__
#define __MY_LIB_MEMORY_HEADER_H__

#include <iostream>
#include <initializer_list>
#include <vector>

#include <cstdint>
#include <cstdlib>
#include <cassert>

#include <my-lib/macros.h>
#include <my-lib/std.h>

namespace Mylib
{
namespace Memory
{

// ---------------------------------------------------

[[nodiscard]] inline void* m_allocate (const size_t size, const uint32_t align)
{
//	std::cout << "m_allocate size " << size << " align " << align << std::endl;
#if __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		return ::operator new(size, std::align_val_t(align));
#endif
//	std::cout << "\tstd align " << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << std::endl;
	return ::operator new(size);
}

// ---------------------------------------------------

inline void m_deallocate (void *p, const size_t size, const uint32_t align)
{
#if __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
		::operator delete(p,
	#if __cpp_sized_deallocation
		size,
	#endif
		std::align_val_t(align));
		return;
	}
#endif
	::operator delete(p
#if __cpp_sized_deallocation
	, size
#endif
	);
}

// ---------------------------------------------------

template <typename T>
consteval uint32_t calculate_alignment ()
{
	if constexpr (alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		return __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	else
		return alignof(T);
}

// ---------------------------------------------------

class Manager
{
public:
	[[nodiscard]] virtual void* allocate (const size_t type_size, const size_t count, const uint32_t align) = 0;
	virtual void deallocate (void *p, const size_t type_size, const size_t count, const uint32_t align) = 0;

	[[nodiscard]] inline void* allocate (const size_t type_size, const size_t count)
	{
		return this->allocate(type_size, count, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	}

	void deallocate (void *p, const size_t type_size, const size_t count)
	{
		this->deallocate(p, type_size, count, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	}

	template <typename T>
	[[nodiscard]] T* allocate_type (const size_t count)
	{
		return static_cast<T*>( this->allocate(sizeof(T), count, calculate_alignment<T>()) );
	}

	template <typename T>
	void deallocate_type (T *p, const size_t count)
	{
		this->deallocate(p, sizeof(T), count, calculate_alignment<T>());
	}
};

// ---------------------------------------------------

class DefaultManager : public Manager
{
public:
	[[nodiscard]] void* allocate (const size_t type_size, const size_t count, const uint32_t align) override final
	{
		return m_allocate(type_size * count, align);
	}

	void deallocate (void *p, const size_t type_size, const size_t count, const uint32_t align) override final
	{
		m_deallocate(p, type_size * count, align);
	}
};

// ---------------------------------------------------


} // end namespace Memory
} // end namespace Mylib

#endif