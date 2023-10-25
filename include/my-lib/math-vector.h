#ifndef __MY_LIB_MATH_VECTOR_HEADER_H__
#define __MY_LIB_MATH_VECTOR_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>
#include <ostream>

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
	using Type = float;

	union {
		Type data[2];

		struct {
			Type x;
			Type y;
		};

		//uint64_t ivalue;
	};

	VectorStorage__ () = default;

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
class VectorStorage__<float, 3>
{
public:
	using Type = float;

	union {
		Type data[3];

		struct {
			Type x;
			Type y;
			Type z;
		};

		//uint64_t ivalue;
	};

	VectorStorage__ () = default;

	VectorStorage__ (const Type x_, const Type y_, const Type z_)
		: x(x_), y(y_), z(z)
	{
	}

	inline void set (const Type x, const Type y, const Type z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

// ---------------------------------------------------

template<>
class VectorStorage__<float, 4>
{
public:
	using Type = float;

	union {
		Type data[4];

		struct {
			Type x;
			Type y;
			Type z;
			Type w;
		};

		//uint64_t ivalue;
	};

	VectorStorage__ () = default;

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

template <typename T, uint32_t dim,
          typename TParent = VectorStorage__<T, dim>>
class Vector : public TParent
{
public:
	using Type = T;

	static_assert(sizeof(TParent) == (dim * sizeof(Type)));

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

	using TParent::TParent;

	template <uint32_t dim_other>
	Vector (const Vector<T, dim_other>& other)
	{
		static_assert(dim_other <= dim);
		for (uint32_t i = 0; i < dim_other; i++)
			this->data[i] = other.data[i];
		for (uint32_t i = dim_other; i < dim; i++)
			this->data[i] = 0;
	}

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
		inline Vector& operator OP (const Type s) \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	inline Type& operator[] (const uint32_t i)
	{
		return this->data[i];
	}

	inline const Type operator[] (const uint32_t i) const
	{
		return this->data[i];
	}

	inline Type length () const
	{
		Type value = 0;
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

template <typename T, uint32_t dim>
using Point = Vector<T, dim>;

// ---------------------------------------------------

using Vector2f = Vector<float, 2>;
using Point2f = Vector2f;

static_assert(sizeof(Vector2f) == (2 * sizeof(float)));

template<typename T>
concept is_Vector2f = std::same_as< typename remove_type_qualifiers<T>::type, Vector2f >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

// ---------------------------------------------------

using Vector3f = Vector<float, 3>;
using Point3f = Vector3f;

static_assert(sizeof(Vector3f) == (3 * sizeof(float)));

template<typename T>
concept is_Vector3f = std::same_as< typename remove_type_qualifiers<T>::type, Vector3f >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

// ---------------------------------------------------

using Vector4f = Vector<float, 4>;
using Point4f = Vector4f;

static_assert(sizeof(Vector4f) == (4 * sizeof(float)));

template<typename T>
concept is_Vector4f = std::same_as< typename remove_type_qualifiers<T>::type, Vector4f >;

// ---------------------------------------------------

template<typename T>
concept is_Vector = is_Vector2f<T> || is_Vector3f<T> || is_Vector4f<T>;

template<typename T>
concept is_Point = is_Vector<T>;

// ---------------------------------------------------

//template <typename T>
//requires is_Vector<Ta> && is_Vector<Tb>
//requires is_Point<T>;
template <typename T, uint32_t dim>
inline T distance (const Point<T, dim>& a, const Point<T, dim>& b)
{
	//static_assert(remove_type_qualifiers<Ta>::type::get_dim() == remove_type_qualifiers<Tb>::type::get_dim());
	return (a - b).length();
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
std::ostream& operator << (std::ostream& o, const Vector<T, dim>& v)
{
	o << "[";
	
	for (uint32_t i = 0; i < v.get_dim(); i++) {
		o << v[i];
		
		if (i < (v.get_dim()-1))
			o << ", ";
	}

	o << "]";

	return o;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif