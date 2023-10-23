#ifndef __MY_LIBS_MATH_VECTOR_HEADER_H__
#define __MY_LIBS_MATH_VECTOR_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>

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

template <typename T, uint32_t dim>
class VectorStorage__;

// ---------------------------------------------------

template<>
class VectorStorage__<float, 2>
{
public:
	using Type = T;

	union {
		Type data[dim];

		struct {
			Type x;
			Type y;
		};

		//uint64_t ivalue;
	};

	VectorStorage__ (const Type x_, const Type y_)
		: x(x_), y(y_)
	{
	}

	inline void set (const Type x, const Type y)
	{
		this->x = x;
		this->y = y;
	}
};

// ---------------------------------------------------

template<>
class VectorStorage__<float, 4>
{
public:
	using Type = T;

	union {
		Type data[dim];

		struct {
			Type x;
			Type y;
			Type z;
			Type w;
		};

		//uint64_t ivalue;
	};

	VectorStorage__ (const Type x_, const Type y_, const Type z_, const Type w_)
		: x(x_), y(y_), z(z), w(w_)
	{
	}

	inline void set (const Type x, const Type y, const Type z, const Type w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

// ---------------------------------------------------

template <typename T, uint32_t dim>
class Vector : public VectorStorage__<T, dim>
{
public:
	using TParent = VectorStorage__<T, dim>;
	using Type = T;

	inline float* get_raw ()
	{
		return this->data;
	}

	inline const float* get_raw () const
	{
		return this->data;
	}

	consteval static uint32_t get_dim ()
	{
		return dim;
	}

	// ------------------------ Constructors

	Vector () = default;

	using TParent::VectorStorage__;

	// ------------------------ operator=

/*	Vector& operator= (const Vector& other)
	{
		this->ivalue = other.ivalue;
		return *this;
	}

	Vector& operator= (Vector&& other)
	{
		this->ivalue = other.ivalue;
		return *this;
	}

	Vector& operator= (const float *v)
	{
		this->ivalue = *(reinterpret_cast<const uint64_t*>(v));
		return *this;
	}*/

	// ------------------------ Other stuff

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		inline Vector& operator OP (const Vector& other) \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				this->data[i] OP other.data[i]; \
			return *this; \
		} \
		inline Vector& operator OP (const T s) \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	inline float& operator[] (const uint32_t i)
	{
		return this->data[i];
	}

	inline const float operator[] (const uint32_t i) const
	{
		return this->data[i];
	}

	inline float length () const
	{
		T value = 0;
		for (uint32_t i = 0; i < dim; i++)
			value += this->data[i] * this->data[i];
		return std::sqrt(value);
	}
};

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t dim> \
	inline Vector<T, dim> operator OP (const Vector<T, dim>& a, const Vector<T, dim>& b) \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r.data[i] = a.data[i] OP b.data[i]; \
		return r; \
	} \
	template <typename T, uint32_t dim> \
	inline Vector<T, dim> operator OP (const Vector<T, dim>& a, const T s) \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r.data[i] = a.data[i] OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )
MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

template <typename T, uint32_t dim> \
inline Vector<T, dim> operator- (const Vector<T, dim>& v)
{
	Vector<T, dim> r;
	for (uint32_t i = 0; i < dim; i++) \
		r.data[i] = -v.data[i];
	return r;
}

template <typename T, uint32_t dim>
inline T dot_product (const Vector<T, dim>& a, const Vector<T, dim>& b)
{
	T value = 0;
	for (uint32_t i = 0; i < dim; i++)
		value += a[i] * b[i];
	return value;
}

// ---------------------------------------------------

using Vector2df = Vector<float, 2>;
using Point2df = Vector2df;

static_assert(sizeof(Vector2df) == (2 * sizeof(float)));
static_assert(sizeof(Vector2df) == sizeof(uint64_t));
static_assert(sizeof(Vector2df) == 8);

template<typename T>
concept is_Vector2df = std::same_as< typename remove_type_qualifiers<T>::type, Vector2df >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

// ---------------------------------------------------

using Vector4df = Vector<float, 4>;
using Point4df = Vector4df;

static_assert(sizeof(Vector4df) == (4 * sizeof(float)));
static_assert(sizeof(Vector4df) == (2 * sizeof(uint64_t)));
static_assert(sizeof(Vector4df) == 16);

template<typename T>
concept is_Vector4df = std::same_as< typename remove_type_qualifiers<T>::type, Vector4df >;

// ---------------------------------------------------

template<typename T>
concept is_Vector = is_Vector2df<T> || is_Vector4df<T>;

template<typename T>
concept is_Point = is_Vector<T>;

// ---------------------------------------------------

template <typename T>
//requires is_Vector<Ta> && is_Vector<Tb>
requires is_Point<T>;
inline T::Type distance (const T& a, const T& b)
{
	//static_assert(remove_type_qualifiers<Ta>::type::get_dim() == remove_type_qualifiers<Tb>::type::get_dim());
	const T d = a - b;
	return d.length();
}

// ---------------------------------------------------

template<typename T>
requires is_Vector<T>
void print (const T& v, std::ostream& out=std::cout)
{
	out << "[";
	
	for (uint32_t i = 0; i < T::get_dim(); i++) {
		out << v(i);
		
		if (i < (T::get_dim()-1))
			out << ", ";
	}

	out << "]";
}

template<typename T>
requires is_Vector<T>
void println (const T& v, std::ostream& out=std::cout)
{
	print(v, out);
	out << std::endl;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif