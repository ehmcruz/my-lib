#ifndef __MY_LIB_MATH_HEADER_H__
#define __MY_LIB_MATH_HEADER_H__

#include <numeric>

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

	throw Mylib::Exception(Mylib::build_str_from_stream("Mylib::base2_log_of_integer\nNumber ", value, " cant be zero"));

	return 0;
}

auto radians_to_degrees (const auto radians) -> decltype(radians)
{
	using Type = decltype(radians);
	return radians * static_cast<Type>(180) / std::numbers::pi_v<Type>;
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif