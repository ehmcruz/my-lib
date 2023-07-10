#ifndef __MY_LIBS_MATH_VECTOR_HEADER_H__
#define __MY_LIBS_MATH_VECTOR_HEADER_H__

#include <my-lib/std.h>

namespace Mylib
{
namespace Math
{

// ---------------------------------------------------

template <uint32_t dim>
class Vector
{
	// TODO
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
	};

	Vector () = default;

	inline Vector (const Vector& other)
	{
		this->data[0] = other.data[0];
		this->data[1] = other.data[1];
	}

	inline Vector (Vector&& other)
	{
		this->data[0] = other.data[0];
		this->data[1] = other.data[1];
	}

	inline Vector (const float x_, const float y_)
		: x(x_), y(y_)
	{
	}

	Vector& operator= (const Vector& other)
	{
		this->data[0] = other.data[0];
		this->data[1] = other.data[1];
		return *this;
	}

	Vector& operator= (Vector&& other)
	{
		this->data[0] = other.data[0];
		this->data[1] = other.data[1];
		return *this;
	}

	Vector& operator= (const float *v)
	{
		this->data[0] = v[0];
		this->data[1] = v[1];
		return *this;
	}

	inline void set (const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}
};

static_assert(sizeof(Vector<2>) == (2 * sizeof(float)));

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif