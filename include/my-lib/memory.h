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

class Manager
{
public:
	[[nodiscard]] virtual void* allocate (const size_t size, const std::align_val_t align) = 0;
	virtual void deallocate (const void *p, const size_t size, const std::align_val_t align) = 0;

	[[nodiscard]] inline void* allocate (const size_t size)
	{
		return this->allocate(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	}

	void deallocate (const void *p, const size_t size)
	{
		this->deallocate(p, size, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	}

	template <typename T>
	[[nodiscard]] T* allocate_type (const size_t count)
	{
		std::align_val_t align;

		if (alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
			align = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
		else
			align = std::align_val_t(alignof(T));
		
		return static_cast<T*>( this->allocate(sizeof(T) * count, align) );
	}

	template <typename T>
	void deallocate_type (const T *p, const size_t count)
	{
		std::align_val_t align;

		if (alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
			align = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
		else
			align = std::align_val_t(alignof(T));
		
		this->deallocate(p, sizeof(T) * count, align);
	}
};

// ---------------------------------------------------

class StdManager : public Manager
{
public:
	[[nodiscard]] virtual void* allocate (const size_t size, const std::align_val_t align) override;
	virtual void deallocate (const void *p, const size_t size, const std::align_val_t align) override;
};

// ---------------------------------------------------


} // end namespace Memory
} // end namespace Mylib

#endif