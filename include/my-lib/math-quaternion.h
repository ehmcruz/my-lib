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
	using Vector = Mylib::Math::Vector<T, 3>;

	// We store in this format (x, y, z, w), where x,y,z are the vector part,
	// and w is the scalar part, so that we can use the same memory layout
	// as in GLSL.
	union {
		struct {
			Vector v;
			T w__;
		};
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		T data[4];
	};

	// ------------------------ Constructors

	Quaternion () noexcept = default;

	Quaternion (const Vector& v_, const T w_) noexcept
		: v(v_), w__(w_)
	{
	}

	Quaternion (const T x_, const T y_, const T z_, const T w_) noexcept
		: x(x_), y(y_), z(z_), w(w_)
	{
	}

	// Create a scalar (or real) quaternion.
	// The vector part is set to zero.

	Quaternion (const T w_) noexcept
		: x(0), y(0), z(0), w(w_)
	{
	}

	// Create a vector (or imaginary, or pure) quaternion.
	// The scalar part is set to zero.

	Quaternion (const Vector& v_) noexcept
		: v(v_), w__(0)
	{
	}

	// ------------------------ operator=

	// use default ones, so no need to define them

	// ------------------------ Other stuff

	constexpr Type& operator[] (const uint32_t i) noexcept
	{
		static_assert(sizeof(Quaternion) == (4 * sizeof(Type)));
		return this->data[i];
	}

	constexpr Type operator[] (const uint32_t i) const noexcept
	{
		static_assert(sizeof(Quaternion) == (4 * sizeof(Type)));
		return this->data[i];
	}

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Quaternion& operator OP (const Quaternion& other) noexcept \
		{ \
			for (uint32_t i = 0; i < 4; i++) \
				this->data[i] OP other[i]; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Quaternion& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < 4; i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	// ---------------------------------------------------

	constexpr Quaternion& operator *= (const Quaternion& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	// ---------------------------------------------------

	constexpr T length_squared () const noexcept
	{
		Type value = 0;
		for (uint32_t i = 0; i < 4; i++)
			value += this->data[i] * this->data[i];
		return value;
	}

	constexpr T length () const noexcept
	{
		return std::sqrt(this->length_squared());
	}

	constexpr void normalize () noexcept
	{
		const Type len = this->length();
		for (uint32_t i = 0; i < 4; i++)
			this->data[i] /= len;
	}

	constexpr void conjugate () noexcept
	{
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
	}

	/*
		If the quaternion is normalized, then its inverse is equal to its conjugate.
	*/

	constexpr void invert_normalized () noexcept
	{
		this->conjugate();
	}

	constexpr void invert () noexcept
	{
		const Type len = this->length_squared();
		this->conjugate();
		for (uint32_t i = 0; i < 4; i++)
			this->data[i] /= len;
	}

	// ---------------------------------------------------

	constexpr void set_zero () noexcept
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	constexpr void set_identity () noexcept
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 1;
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

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

static_assert(sizeof(Quaternionf) == (4 * sizeof(float)));
static_assert(sizeof(Quaterniond) == (4 * sizeof(double)));

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T> \
	constexpr Quaternion<T> operator OP (const Quaternion<T>& a, const Quaternion<T>& b) noexcept \
	{ \
		Quaternion<T> r; \
		for (uint32_t i = 0; i < 4; i++) \
			r[i] = a[i] OP b[i]; \
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
		for (uint32_t i = 0; i < 4; i++) \
			r[i] = a[i] OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

// ---------------------------------------------------

template <typename T>
constexpr Quaternion<T> operator- (const Quaternion<T>& q) noexcept
{
	Quaternion<T> r;
	for (uint32_t i = 0; i < 4; i++)
		r[i] = -q[i];
	return r;
}

// ---------------------------------------------------

template <typename T>
constexpr Quaternion<T> normalize (const Quaternion<T>& q) noexcept
{
	return q / q.length();
}

// ---------------------------------------------------

template <typename T>
constexpr Quaternion<T> conjugate (const Quaternion<T>& q) noexcept
{
	return Quaternion<T>(-q.x, -q.y, -q.z, q.w);
}

// ---------------------------------------------------

/*
	If the quaternion is normalized, then its inverse is equal to its conjugate.
*/

template <typename T>
constexpr Quaternion<T> invert_normalized (const Quaternion<T>& q) noexcept
{
	return conjugate(q);
}

template <typename T>
constexpr Quaternion<T> invert (const Quaternion<T>& q) noexcept
{
	return conjugate(q) / q.length_squared();
}

// ---------------------------------------------------

template <typename T>
constexpr Quaternion<T> operator* (const Quaternion<T>& a, const Quaternion<T>& b) noexcept
{
	Quaternion<T> r;
	r.s = a.s * b.s - dot_product(a.v, b.v);
	r.v = (b.v * a.s) + (a.v * b.s) + cross_product(a.v, b.v);
	return r;
}

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