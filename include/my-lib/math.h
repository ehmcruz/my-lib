#ifndef __MY_LIB_MATH_HEADER_H__
#define __MY_LIB_MATH_HEADER_H__

#include <my-lib/std.h>

namespace Mylib
{
namespace Math
{

// ---------------------------------------------------

template <typename T>
T base2_log_of_integer (const T value)
{
	const uint32_t nbits = sizeof(T) * 8;

	for (uint32_t pos = 0; pos < nbits; pos++) {
		if ((value >> pos) & 0x01)
			return pos;
	}

	throw Mylib::Exception("Mylib::base2_log_of_integer\nNumber ", value, " cant be zero");

	return 0;
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif