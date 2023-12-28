#ifndef __MY_LIB_MATH_QUATERNION_HEADER_H__
#define __MY_LIB_MATH_QUATERNION_HEADER_H__

#include <concepts>
#include <type_traits>
#include <ostream>

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/math-vector.h>
#include <my-lib/math-matrix.h>

namespace Mylib
{
namespace Math
{

#ifdef MYLIB_MATH_BUILD_OPERATION
#error nooooooooooo
#endif

// ---------------------------------------------------

template <typename T>
class Quaternion
{
public:
	using Type = T;
	using Vector = Vector<T, 3>;

	T s;
	Vector v;

	// ------------------------ Constructors

	Quaternion () noexcept = default;

	Quaternion (const T s_, const Vector& v_) noexcept
		: s(s_), v(v_)
	{
	}

	Quaternion (const T s_, const T x, const T y, const T z) noexcept
		: s(s_), v(x, y, z)
	{
	}

	// Create a scalar (or real) quaternion.
	// The vector part is set to zero.

	Quaternion (const T s_) noexcept
		: s(s_)
	{
		this->v.set_zero();
	}

	// Create a vector (or imaginary, or pure) quaternion.
	// The scalar part is set to zero.

	Quaternion (const Vector& v_) noexcept
		: s(0), v(v_)
	{
	}

	// ------------------------ operator=

	// use default ones, so no need to define them

	// ------------------------ Other stuff

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Quaternion& operator OP (const Quaternion& other) noexcept \
		{ \
			this->s OP other.s; \
			this->v OP other.v; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Quaternion& operator OP (const Type s) noexcept \
		{ \
			this->s OP s; \
			this->v OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	// ---------------------------------------------------

	constexpr Quaternion& operator *= (const Quaternion& other) noexcept
	{
		Quaternion r;
		r.s = this->s * other.s - dot_product(this->v, other.v);
		r.v = (other.v * this->s) + (this->v * other.s) + cross_product(this->v, other.v);
		*this = r;
		return *this;
	}

	// ---------------------------------------------------

	constexpr void set_zero () noexcept
	{
		this->s = 0;
		this->v.set_zero();
	}

	constexpr void set_identity () noexcept
	{
		this->s = 1;
		this->v.set_zero();
	}

	// ---------------------------------------------------

	static consteval Quaternion zero () noexcept
	{
		Quaternion q;
		q.set_zero();
		return q;
	}

	static consteval Quaternion identity () noexcept
	{
		Quaternion q;
		q.set_identity();
		return q;
	}
};

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T> \
	constexpr Quaternion<T> operator OP (const Quaternion<T>& a, const Quaternion<T>& b) noexcept \
	{ \
		Quaternion<T> r; \
		r.s = a.s OP b.s; \
		r.v = a.v OP b.v; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T> \
	constexpr Quaternion<T> operator OP (const Quaternion<T>& a, const T s) noexcept \
	{ \
		Quaternion<T> r; \
		r.s = a.s OP s; \
		r.v = a.v OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

// ---------------------------------------------------

template <typename T>
constexpr Quaternion operator *= (const Quaternion& other) noexcept

// ---------------------------------------------------

template <typename T>
std::ostream& operator << (std::ostream& out, const Quaternion<T>& q)
{
	out << "[" << q.s << ", " << q.v.x << ", " << q.v.y << ", " << q.v.z << "]";
	return out;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif