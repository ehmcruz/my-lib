#ifndef __MY_LIB_MATH_QUATERNION_HEADER_H__
#define __MY_LIB_MATH_QUATERNION_HEADER_H__

#include <concepts>
#include <type_traits>
#include <ostream>
#include <utility>

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
	using Vector = Mylib::Math::Vector<VectorStorage__<Type, 3>>;

	constexpr static Type fp (const auto v) noexcept
	{
		return static_cast<Type>(v);
	}

	// We store in this format (x, y, z, w), where x,y,z are the vector part,
	// and w is the scalar part, so that we can use the same memory layout
	// as in GLSL.

	Vector v;
	Type w;

	// ------------------------ Constructors

	constexpr Quaternion () noexcept
		: v(Vector::zero()), w(0)
	{
	}

	constexpr Quaternion (const Vector& v_, const T w_) noexcept
		: v(v_), w(w_)
	{
	}

	constexpr Quaternion (const T x_, const T y_, const T z_, const T w_) noexcept
		: v(x_, y_, z_), w(w_)
	{
	}

	// Create a scalar (or real) quaternion.
	// The vector part is set to zero.

	constexpr Quaternion (const T w_) noexcept
		: v(Vector::zero()), w(w_)
	{
	}

	// Create a vector (or imaginary, or pure) quaternion.
	// The scalar part is set to zero.

	constexpr Quaternion (const Vector& v_) noexcept
		: v(v_), w(0)
	{
	}

	// ------------------------ operator=

	// use default ones, so no need to define them

	// ------------------------ Other stuff

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Quaternion& operator OP (this Quaternion& self, const Quaternion& other) noexcept \
		{ \
			self.v OP other.v; \
			self.w OP other.w; \
			return self; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Quaternion& operator OP (this Quaternion& self, const Type s) noexcept \
		{ \
			self.v OP s; \
			self.w OP s; \
			return self; \
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

	constexpr T length_squared (this const Quaternion& self) noexcept
	{
		Type value = dot_product(self.v, self.v);
		value += self.w * self.w;
		return value;
	}

	constexpr T length () const noexcept
	{
		return std::sqrt(this->length_squared());
	}

	// ---------------------------------------------------

	/*
		Converts the quaternion to a pair of a normalized axis-vector and an angle.
		It considers the quaternion as a rotation quaternion (unit quaternion length=1).
	*/

	constexpr std::pair<Vector, T> to_axis_angle () const noexcept
	{
		Quaternion<T> q = (this->w > 0) ? *this : Quaternion<T>(-(*this));
		Vector axis;
		T angle;
		
		const T length = q.v.normalize();

		// In case length is zero, normalize leaves NaNs in axis.
		// This happens at angle = 0 and 360.
		// All axes are correct, so any will do.

		if (length == 0) [[unlikely]]
			axis.set(1, 0, 0);
		else
			axis = q.v;

		angle = 2 * std::atan2(length, this->w);
		// angle = std::acos(this->w) * fp(2);

		return std::make_pair(axis, angle);

/*		const T angle = std::acos(this->w) * 2;
		const T s = std::sqrt(1 - this->w * this->w);

		if (s < 0.001f)
			return std::make_pair(Vector(1, 0, 0), 0);

		return std::make_pair(this->v / s, angle);*/
	}

	// ---------------------------------------------------

	constexpr void set_rotation (const Vector& axis, const T angle) noexcept
	{
		const T half_angle = angle / fp(2);
		this->v = Mylib::Math::normalize(axis) * std::sin(half_angle);
		this->w = std::cos(half_angle);
	}

	// ---------------------------------------------------

	constexpr void set_rotation (Vector start, Vector end) noexcept
	{
		start.normalize();
		end.normalize();

		const T dot_product = Mylib::Math::dot_product(start, end);

		// From MathFu library:
		// Any rotation < 0.1 degrees is treated as no rotation
		// in order to avoid division by zero errors.
		// So we early-out in cases where it's less than 0.1 degrees.
		// cos( 0.1 degrees) = 0.99999847691

		if (dot_product >= fp(0.99999847691)) {
			this->set_identity();
			return;
		}

		// From MathFu library:
		// If the vectors point in opposite directions, return a 180 degree
		// rotation, on an arbitrary axis.

		if (dot_product <= fp(-0.99999847691)) {
			this->v = orthogonal_vector(start);
			this->w = 0;
			return;
		}
		
		// Degenerate cases have been handled, so if we're here, we have to
		// actually compute the angle we want.

		this->v = cross_product(start, end);
		this->w = fp(1.0) + dot_product;

		this->normalize();
	}

	// ---------------------------------------------------

	// normalize returns the length before normalization.
	// Got the idea from the MathFu library.

	constexpr Type normalize (this Quaternion& self) noexcept
	{
		const Type len = self.length();
		self.v /= len;
		self.w /= len;
		return len;
	}

	constexpr void conjugate () noexcept
	{
		this->v.negate();
	}

	/*
		If the quaternion is normalized, then its inverse is equal to its conjugate.
	*/

	constexpr void invert_normalized () noexcept
	{
		this->conjugate();
	}

	constexpr void invert (this Quaternion& self) noexcept
	{
		const Type len = self.length_squared();
		self.conjugate();
		self.v /= len;
		self.w /= len;
	}

	// ---------------------------------------------------

	constexpr void set_zero () noexcept
	{
		this->v.set_zero();
		this->w = 0;
	}

	constexpr void set_identity () noexcept
	{
		this->v.set_zero();
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

	static constexpr Quaternion rotation (const Vector& axis, const T angle) noexcept
	{
		Quaternion q;
		q.set_rotation(axis, angle);
		return q;
	}

	static constexpr Quaternion rotation (const Vector& start, const Vector& end) noexcept
	{
		Quaternion q;
		q.set_rotation(start, end);
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
		r.v = a.v OP b.v; \
		r.w = a.w OP b.w; \
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
		r.v = a.v OP s; \
		r.w = a.w OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

// ---------------------------------------------------

template <typename T>
constexpr Quaternion<T> operator- (const Quaternion<T>& q) noexcept
{
	Quaternion<T> r;
	r.v = -q.v;
	r.w = -q.w;
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
	return Quaternion<T>(-q.v, q.w);
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
constexpr Quaternion<T> operator* (const Quaternion<T>& q1, const Quaternion<T>& q2) noexcept
{
	Quaternion<T> r;

	// Hamilton product

	enum { x, y, z };

	r.v[x] = (q1.w * q2.v[x]) + (q1.v[x] * q2.w) + (q1.v[y] * q2.v[z]) - (q1.v[z] * q2.v[y]);
	r.v[y] = (q1.w * q2.v[y]) - (q1.v[x] * q2.v[z]) + (q1.v[y] * q2.w) + (q1.v[z] * q2.v[x]);
	r.v[z] = (q1.w * q2.v[z]) + (q1.v[x] * q2.v[y]) - (q1.v[y] * q2.v[x]) + (q1.v[z] * q2.w);
	r.w = (q1.w * q2.w) - (q1.v[x] * q2.v[x]) - (q1.v[y] * q2.v[y]) - (q1.v[z] * q2.v[z]);
	//r.v = (b.v * a.w) + (a.v * b.w) + cross_product(a.v, b.v);
	//r.w = a.w * b.w - dot_product(a.v, b.v);

	return r;
}

// ---------------------------------------------------

template <typename Tvector>
constexpr Vector<Tvector> rotate (const Quaternion<typename Tvector::Type>& q, Vector<Tvector> v) noexcept
	requires (Tvector::dim == 3)
{
	v.rotate(q);
	return v;
}

// ---------------------------------------------------

template <typename T>
std::ostream& operator << (std::ostream& out, const Quaternion<T>& q)
{
	enum { x, y, z };
	out << "[" << q.v[x] << ", " << q.v[y] << ", " << q.v[z] << ", " << q.w << "]";
	return out;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif