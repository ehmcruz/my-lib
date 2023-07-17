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

template <uint32_t dim>
class Vector
{
public:
	consteval static uint32_t get_dim ()
	{
		return dim;
	}

	float data[dim];

	Vector () = default;

	inline Vector (const Vector& other)
	{
		for (uint32_t i = 0; i < dim; i++)
			this->data[i] = other.data[i];
	}

	inline Vector (const float *data)
	{
		for (uint32_t i = 0; i < dim; i++)
			this->data[i] = data[i];
	}

	Vector& operator= (const Vector& other)
	{
		for (uint32_t i = 0; i < dim; i++)
			this->data[i] = other.data[i];
		return *this;
	}

	Vector& operator= (const float *v)
	{
		for (uint32_t i = 0; i < dim; i++)
			this->data[i] = v[i];
		return *this;
	}
};

// ---------------------------------------------------

template<>
class Vector<2>
{
public:
	union {
		float data[2];

		struct {
			float x;
			float y;
		};

		//uint64_t ivalue;
	};

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
		return 2;
	}

	// ------------------------ Constructors

	Vector () = default;

/*	inline Vector (const Vector& other)
	{
		this->ivalue = other.ivalue;
	}

	inline Vector (Vector&& other)
	{
		this->ivalue = other.ivalue;
	}

	inline Vector (const float *v)
	{
		this->ivalue = *(reinterpret_cast<const uint64_t*>(v));
	}
*/
	inline Vector (const float x_, const float y_)
		: x(x_), y(y_)
	{
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

	inline void set (const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		inline Vector& operator OP (const Vector& other) \
		{ \
			this->data[0] OP other.data[0]; \
			this->data[1] OP other.data[1]; \
			return *this; \
		} \
		inline Vector& operator OP (const float s) \
		{ \
			this->data[0] OP s; \
			this->data[1] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	inline float length () const
	{
		return std::sqrt(this->x * this->x + this->y * this->y);
	}

	inline float& operator[] (const uint32_t i)
	{
		return this->data[i];
	}

	inline const float operator[] (const uint32_t i) const
	{
		return this->data[i];
	}

	inline const float operator() (const uint32_t i) const
	{
		return this->data[i];
	}
};

using Vector2d = Vector<2>;

static_assert(sizeof(Vector2d) == (2 * sizeof(float)));
static_assert(sizeof(Vector2d) == sizeof(uint64_t));
static_assert(sizeof(Vector2d) == 8);

template<typename T>
concept is_Vector2d = std::same_as< typename remove_type_qualifiers<T>::type, Vector2d >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	inline Vector2d operator OP (const Vector2d& a, const Vector2d& b) \
	{ \
		Vector2d r; \
		r.data[0] = a.data[0] OP b.data[0]; \
		r.data[1] = a.data[1] OP b.data[1]; \
		return r; \
	} \
	inline Vector2d operator OP (const Vector2d& a, const float s) \
	{ \
		Vector2d r; \
		r.data[0] = a.data[0] OP s; \
		r.data[1] = a.data[1] OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )
MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

inline Vector2d operator- (const Vector2d& v)
{
	Vector2d r;
	r.data[0] = -v.data[0];
	r.data[1] = -v.data[1];
	return r;
}

inline float dot_product (const Vector2d& a, const Vector2d& b)
{
	return a.x * b.x + a.y * b.y;
}

/*#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(NAME, CODE) \
	inline float NAME (const Vector2d& a, const Vector2d& b) \
	{ \
		CODE \
	} \
	inline float NAME (const Vector2d&& a, const Vector2d& b) \
	{ \
		CODE \
	} \
	inline float NAME (const Vector2d& a, const Vector2d&& b) \
	{ \
		CODE \
	} \
	inline float NAME (const Vector2d&& a, const Vector2d&& b) \
	{ \
		CODE \
	}

MYLIB_MATH_BUILD_OPERATION(distance, Vector2d d = a - b; return d.length();)*/

// ---------------------------------------------------

template<>
class Vector<4>
{
public:
	union {
		float data[4];

		struct {
			float x;
			float y;
			float z;
			float w;
		};

		//uint64_t ivalue[2];
	};

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
		return 4;
	}

	// ------------------------ Constructors

	Vector () = default;

	Vector (const Vector2d& v2d)
	{
		this->data[0] = v2d.data[0];
		this->data[1] = v2d.data[1];
		this->data[2] = 0.0f;
		this->data[3] = 1.0f;
	}

	inline float& operator[] (const uint32_t i)
	{
		return this->data[i];
	}

	inline const float operator[] (const uint32_t i) const
	{
		return this->data[i];
	}

	inline const float operator() (const uint32_t i) const
	{
		return this->data[i];
	}
};

using Vector4d = Vector<4>;

static_assert(sizeof(Vector4d) == (4 * sizeof(float)));
static_assert(sizeof(Vector4d) == (2 * sizeof(uint64_t)));
static_assert(sizeof(Vector4d) == 16);

template<typename T>
concept is_Vector4d = std::same_as< typename remove_type_qualifiers<T>::type, Vector4d >;

// ---------------------------------------------------

template<typename T>
concept is_Vector = is_Vector2d<T> || is_Vector4d<T>;

// ---------------------------------------------------

template <typename Ta, typename Tb>
requires is_Vector<Ta> && is_Vector<Tb>
inline float distance (const Ta& a, const Tb& b)
{
	static_assert(remove_type_qualifiers<Ta>::type::get_dim() == remove_type_qualifiers<Tb>::type::get_dim());
	Vector2d d = a - b;
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