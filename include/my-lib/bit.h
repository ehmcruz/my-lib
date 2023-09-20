#ifndef __MY_LIB_BIT_HEADER_H__
#define __MY_LIB_BIT_HEADER_H__

#include <cstdint>

namespace Mylib
{

// ---------------------------------------------------

template <typename T>
inline T extract_bits (const T v, const uint32_t bstart, const uint32_t blength)
{
	const T mask = (1 << blength) - 1;

	return (v >> bstart) & mask;
}

template <typename T>
inline T set_bits (const T v, const uint32_t bstart, const uint32_t blength, const T bits)
{
	const T mask = ((1 << blength) - 1) << bstart;

	return (v & ~mask) | (bits << bstart);
}

// ---------------------------------------------------

} // end namespace Mylib

#endif