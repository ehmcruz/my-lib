#ifndef __MY_LIBS_ANY_HEADER_H__
#define __MY_LIBS_ANY_HEADER_H__

#include <type_traits>

#include <cstdint>
#include <cstring>

#include <my-lib/macros.h>
#include <my-lib/std.h>

namespace Mylib
{

// ---------------------------------------------------

/*
	I don't like std::any because the C++ standard doesn't require
	it support static memory allocation.
	So I wrote this simpler version of it with static memory.
*/

template <uint32_t minimum_storage_size, uint32_t alignment=__STDCPP_DEFAULT_NEW_ALIGNMENT__>
class Any
{
public:
	consteval static uint32_t size ()
	{
		return ((minimum_storage_size % alignment) == 0)
		? minimum_storage_size
		: static_cast<uint32_t>((minimum_storage_size + alignment) / alignment) * alignment;
	}

private:
	uint8_t data[ size() ];

public:
	template <typename T>
	inline Any& operator= (const T& value)
	{
		static_assert(sizeof(T) <= size());
		memcpy(this, &value, sizeof(T));
		return *this;
	}

	// T must be explicitly set when calling
	template <typename T>
	inline T& get_value ()
	{
		static_assert(sizeof(T) <= size());
		return *(reinterpret_cast<T*>(this));
	}

	// T must be explicitly set when calling
	template <typename T>
	inline const T& get_value () const
	{
		static_assert(sizeof(T) <= size());
		return *(reinterpret_cast<T*>(this));
	}
} __attribute__((aligned(alignment)));

// ---------------------------------------------------

} // end namespace Mylib

#endif