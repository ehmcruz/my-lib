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
#if __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		return ::operator new(size, std::align_val_t(align));
#endif

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
		if constexpr (alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
			return static_cast<T*>( this->allocate(sizeof(T), count, __STDCPP_DEFAULT_NEW_ALIGNMENT__) );
		else
			return static_cast<T*>( this->allocate(sizeof(T), count, alignof(T)) );
	}

	template <typename T>
	void deallocate_type (T *p, const size_t count)
	{
		if constexpr (alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
			this->deallocate(p, sizeof(T), count, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
		else
			this->deallocate(p, sizeof(T), count, alignof(T));
	}
};

// ---------------------------------------------------

class DefaultManager : public Manager
{
public:
	[[nodiscard]] void* allocate (const size_t type_size, const size_t count, const uint32_t align) override;
	void deallocate (void *p, const size_t type_size, const size_t count, const uint32_t align) override;
};

// ---------------------------------------------------


} // end namespace Memory
} // end namespace Mylib

#endif