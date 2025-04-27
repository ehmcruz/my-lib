#ifndef __MY_LIB_MATH_VECTOR_HEADER_H__
#define __MY_LIB_MATH_VECTOR_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>
#include <ostream>
#include <algorithm>

#include <cmath>

#include <my-lib/std.h>

namespace Mylib
{
namespace Math
{

#ifdef MYLIB_MATH_BUILD_OPERATION
#error nooooooooooo
#endif

// ---------------------------------------------------

template <typename T>
class Quaternion;

// ---------------------------------------------------

template <typename T, uint32_t dim>
class VectorStorage__;

// ---------------------------------------------------

template <typename T>
class VectorStorage__<T, 2>
{
public:
	using Type = T;

	union {
		struct {
			Type x;
			Type y;
		};

		Type data[2];
	};

	constexpr VectorStorage__ () noexcept
		: data{0, 0}
	{
	}

	constexpr VectorStorage__ (const Type x_, const Type y_) noexcept
		: data{x_, y_}
	{
	}

	constexpr void set (const Type x, const Type y) noexcept
	{
		this->data[0] = x;
		this->data[1] = y;
	}
};

// ---------------------------------------------------

template <typename T>
class VectorStorage__<T, 3>
{
public:
	using Type = T;

	union {
		struct {
			Type x;
			Type y;
			Type z;
		};

		struct {
			Type r;
			Type g;
			Type b;
		};

		Type data[3];
	};

	constexpr VectorStorage__ () noexcept
		: data{0, 0, 0}
	{
	}

	constexpr VectorStorage__ (const Type x_, const Type y_, const Type z_) noexcept
		: data{x_, y_, z_}
	{
	}

	constexpr void set (const Type x, const Type y, const Type z) noexcept
	{
		this->data[0] = x;
		this->data[1] = y;
		this->data[2] = z;
	}
};

// ---------------------------------------------------

template <typename T>
class VectorStorage__<T, 4>
{
public:
	using Type = T;

	union {
		struct {
			Type x;
			Type y;
			Type z;
			Type w;
		};

		struct {
			Type r;
			Type g;
			Type b;
			Type a;
		};

		Type data[4];
	};

	constexpr VectorStorage__ () noexcept
		: data{0, 0, 0, 0}
	{
	}

	constexpr VectorStorage__ (const Type x_, const Type y_, const Type z_, const Type w_) noexcept
		: data{x_, y_, z_, w_}
	{
	}

	constexpr void set (const Type x, const Type y, const Type z, const Type w) noexcept
	{
		this->data[0] = x;
		this->data[1] = y;
		this->data[2] = z;
		this->data[3] = w;
	}
};

// ---------------------------------------------------

template <typename T, uint32_t dim,
          typename TParent = VectorStorage__<T, dim>>
class Vector : public TParent
{
public:
	using Type = T;

	static_assert(sizeof(TParent) == (dim * sizeof(Type)));

	constexpr Type* get_raw () noexcept
	{
		return this->data;
	}

	constexpr const Type* get_raw () const noexcept
	{
		return this->data;
	}

	consteval static uint32_t get_dim () noexcept
	{
		return dim;
	}

	constexpr static Type fp (const auto v) noexcept
	{
		return static_cast<Type>(v);
	}

	// ------------------------ operator[]

	constexpr Type& operator[] (const uint32_t i) noexcept
	{
		static_assert(sizeof(Vector) == (dim * sizeof(Type)));
		//return static_cast<Type*>(this)[i];
		return this->data[i];
	}

	constexpr Type operator[] (const uint32_t i) const noexcept
	{
		static_assert(sizeof(Vector) == (dim * sizeof(Type)));
		//return static_cast<const Type*>(this)[i];
		return this->data[i];
	}

	// ------------------------ Constructors

	using TParent::TParent;

	template <uint32_t dim_other>
	constexpr Vector (const Vector<T, dim_other>& other) noexcept
	{
		static_assert(dim_other <= dim);
		auto& self = *this;
		for (uint32_t i = 0; i < dim_other; i++)
			self[i] = other[i];
		for (uint32_t i = dim_other; i < dim; i++)
			self[i] = 0;
	}

	// ------------------------ operator=

	// use default ones, so no need to define them

	// ------------------------ Other stuff

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Vector& operator OP (this Vector& self, const Vector& other) noexcept \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				self[i] OP other[i]; \
			return self; \
		} \
		constexpr Vector& operator OP (this Vector& self, const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				self[i] OP s; \
			return self; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )


	constexpr Type length_squared (this const Vector& self) noexcept
	{
		Type value = 0;
		for (uint32_t i = 0; i < dim; i++)
			value += self[i] * self[i];
		return value;
	}

	constexpr Type length () const noexcept
	{
		return std::sqrt(this->length_squared());
	}

	constexpr void set_length (this Vector& self, const Type len) noexcept
	{
		const Type ratio = len / self.length();
		for (uint32_t i = 0; i < dim; i++)
			self[i] *= ratio;
	}

	// normalize returns the length of the vector before normalization.
	// Got the idea from the MathFu library.

	constexpr Type normalize (this Vector& self) noexcept
	{
		const Type len = self.length();
		for (uint32_t i = 0; i < dim; i++)
			self[i] /= len;
		return len;
	}

	constexpr void negate (this Vector& self) noexcept
	{
		for (uint32_t i = 0; i < dim; i++)
			self[i] = -self[i];
	}

	constexpr void invert () noexcept
	{
		this->negate();
	}

	constexpr void abs (this Vector& self) noexcept
	{
		for (uint32_t i = 0; i < dim; i++)
			self[i] = std::abs(self[i]);
	}

	constexpr void cross_product (this Vector& self, const Vector& a, const Vector& b) noexcept
	{
		static_assert(dim == 3);
		enum { x, y, z };
		self[x] = a[y] * b[z] - a[z] * b[y];
		self[y] = a[z] * b[x] - a[x] * b[z];
		self[z] = a[x] * b[y] - a[y] * b[x];
	}

	constexpr void rotate (const Quaternion<T>& q) noexcept
	{
		const Quaternion<T> v_(*this); // create a pure quaternion from the vector
		const Quaternion<T> r = (q * v_) * conjugate(q);
		*this = r.v;
	}

	constexpr void project (const Vector& target) noexcept
	{
		*this = target * (dot_product(*this, target) / target.length_squared());
	}

	constexpr void set_zero (this Vector& self) noexcept
	{
		for (uint32_t i = 0; i < dim; i++)
			self[i] = 0;
	}

	static consteval Vector zero () noexcept
	{
		return Vector();
	}
};

// ---------------------------------------------------

template <typename T, uint32_t dim>
using Point = Vector<T, dim>;

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t dim> \
	constexpr Vector<T, dim> operator OP (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r[i] = a[i] OP b[i]; \
		return r; \
	} \
	template <typename T, uint32_t dim> \
	constexpr Vector<T, dim> operator OP (const Vector<T, dim>& a, const T s) noexcept \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r[i] = a[i] OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )
MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )


#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t dim> \
	constexpr Vector<T, dim> operator OP (const T s, const Vector<T, dim>& a) noexcept \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r[i] = s OP a[i]; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )
MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )


template <typename T, uint32_t dim> \
constexpr Vector<T, dim> operator- (Vector<T, dim> v) noexcept
{
	for (uint32_t i = 0; i < dim; i++) \
		v[i] = -v[i];
	return v;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr bool operator== (const Vector<T, dim>& lhs, const Vector<T, dim>& rhs)
{
	for (uint32_t i = 0; i < dim; i++)
		if (lhs[i] != rhs[i])
			return false;
	return true;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr bool operator!= (const Vector<T, dim>& lhs, const Vector<T, dim>& rhs)
{
	return !(lhs == rhs);
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr T dot_product (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept
{
	T value = 0;
	for (uint32_t i = 0; i < dim; i++)
		value += a[i] * b[i];
	return value;
}

// ---------------------------------------------------

template <typename T>
constexpr Vector<T, 3> cross_product (const Vector<T, 3>& a, const Vector<T, 3>& b) noexcept
{
	Vector<T, 3> v;
	v.cross_product(a, b);
	return v;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr Vector<T, dim> abs (Vector<T, dim> v) noexcept
{
	for (uint32_t i = 0; i < dim; i++)
		v[i] = std::abs(v[i]);
	return v;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr Vector<T, dim> max (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept
{
	Vector<T, dim> r;
	for (uint32_t i = 0; i < dim; i++)
		r[i] = std::max(a[i], b[i]);
	return r;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr Vector<T, dim> normalize (const Vector<T, dim>& v) noexcept
{
	return v / v.length();
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr Vector<T, dim> projection (Vector<T, dim> source, const Vector<T, dim>& target) noexcept
{
	source.project(target);
	return source;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr Vector<T, dim> with_length (const Vector<T, dim>& v, const T len) noexcept
{
	return v * (len / v.length());
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr T distance (const Point<T, dim>& a, const Point<T, dim>& b) noexcept
{
	//static_assert(remove_type_qualifiers<Ta>::type::get_dim() == remove_type_qualifiers<Tb>::type::get_dim());
	return (a - b).length();
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr T distance_squared (const Point<T, dim>& a, const Point<T, dim>& b) noexcept
{
	return (a - b).length_squared();
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr T cos_angle_between (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept
{
	return dot_product(a, b) / (a.length() * b.length());
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr T angle_between (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept
{
	return std::acos(cos_angle_between(a, b));
}

// ---------------------------------------------------

// Find an arbitrary vector that is orthogonal to the given vector.
// Copied from the MathFu library.

template <typename T>
constexpr Vector<T, 3> orthogonal_vector (const Vector<T, 3>& v) noexcept
{
	// We start out by taking the cross product of the vector and the x-axis to
	// find something parallel to the input vectors.  If that cross product
	// turns out to be length 0 (i. e. the vectors already lie along the x axis)
	// then we use the y-axis instead.
	
	Vector<T, 3> r = cross_product(Vector<T, 3>(1, 0, 0), v);
	
	// We use a fairly high epsilon here because we know that if this number
	// is too small, the axis we'll get from a cross product with the y axis
	// will be much better and more numerically stable.
	
	if (r.length_squared() < static_cast<T>(0.05))
		r = cross_product(Vector<T, 3>(0, 1, 0), v);

	return r;
}

// ---------------------------------------------------

template <typename T>
struct VectorBasis3
{
	using VectorBasis = VectorBasis3;
	using Vector = Mylib::Math::Vector<T, 3>;

	Vector vx;
	Vector vy;
	Vector vz;

	constexpr static uint32_t get_dim () noexcept
	{
		return 3;
	}

	// rh is right-handed

	constexpr void set_default_rh_orthonormal_basis () noexcept
	{
		this->vx = Vector(1, 0, 0);
		this->vy = Vector(0, 1, 0);
		this->vz = Vector(0, 0, 1);
	}

	constexpr void rotate (const Quaternion<T>& q) noexcept
	{
		this->vx.rotate(q);
		this->vy.rotate(q);
		this->vz.rotate(q);
	}

	static constexpr VectorBasis default_rh_orthonormal_basis () noexcept
	{
		VectorBasis basis;
		basis.set_default_rh_orthonormal_basis();
		return basis;
	}
};

// ---------------------------------------------------

using Vector2f = Vector<float, 2>;
using Point2f = Vector2f;

static_assert(sizeof(Vector2f) == (2 * sizeof(float)));

template <typename T>
concept is_Vector2f = std::same_as< typename remove_type_qualifiers<T>::type, Vector2f >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

// ---------------------------------------------------

using Vector3f = Vector<float, 3>;
using Point3f = Vector3f;

static_assert(sizeof(Vector3f) == (3 * sizeof(float)));

template <typename T>
concept is_Vector3f = std::same_as< typename remove_type_qualifiers<T>::type, Vector3f >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

// ---------------------------------------------------

using Vector4f = Vector<float, 4>;
using Point4f = Vector4f;

static_assert(sizeof(Vector4f) == (4 * sizeof(float)));

template <typename T>
concept is_Vector4f = std::same_as< typename remove_type_qualifiers<T>::type, Vector4f >;

// ---------------------------------------------------

template <typename T>
concept is_Vector = is_Vector2f<T> || is_Vector3f<T> || is_Vector4f<T>;

template <typename T>
concept is_Point = is_Vector<T>;

// ---------------------------------------------------

template <typename T, uint32_t dim>
std::ostream& operator << (std::ostream& out, const Vector<T, dim>& v)
{
	out << "[";
	
	for (uint32_t i = 0; i < dim; i++) {
		out << v[i];
		
		if (i < (dim-1))
			out << ", ";
	}

	out << "]";

	return out;
}

// ---------------------------------------------------

template <typename T>
std::ostream& operator << (std::ostream& out, const VectorBasis3<T>& b)
{
	out << "[" << b.vx << ", " << b.vy << ", " << b.vz << "]";
	return out;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif