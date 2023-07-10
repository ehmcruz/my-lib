#ifndef __MY_LIBS_MATH_VECTOR_HEADER_H__
#define __MY_LIBS_MATH_VECTOR_HEADER_H__

#include <cmath>

#include <my-lib/std.h>

namespace Mylib
{
namespace Math
{

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

	inline Vector (Vector&& other)
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

	Vector& operator= (Vector&& other)
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

	inline Vector& operator += (const Vector& other)
	{
		this->data[0] += other.data[0];
		this->data[1] += other.data[1];
		return *this;
	}

	inline Vector& operator *= (const float s)
	{
		this->data[0] *= s;
		this->data[1] *= s;
		return *this;
	}

	inline float length () const
	{
		return std::sqrt(this->x * this->x + this->y * this->y);
	}

	static inline float dot_product (const Vector& a, const Vector& b)
	{
		return a.x * b.x + a.y * b.y;
	}
};

using Vector2d = Vector<2>;

static_assert(sizeof(Vector2d) == (2 * sizeof(float)));
static_assert(sizeof(Vector2d) == sizeof(uint64_t));
static_assert(sizeof(Vector2d) == 8);

inline Vector2d operator* (const Vector2d& v, const float s)
{
	Vector2d r;
	r.data[0] = v.data[0] * s;
	r.data[1] = v.data[1] * s;
	return r;
}

inline Vector2d operator- (const Vector2d& a, const Vector2d& b)
{
	Vector2d r;
	r.data[0] = a.data[0] - b.data[0];
	r.data[1] = a.data[1] - b.data[1];
	return r;
}

inline float distance (const Vector2d& a, const Vector2d& b)
{
	Vector2d d = a - b;
	return d.length();
}

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

	// ------------------------ Constructors

	Vector () = default;

	Vector (const Vector2d& v2d)
	{
		this->data[0] = v2d.data[0];
		this->data[1] = v2d.data[1];
		this->data[2] = 0.0f;
		this->data[3] = 1.0f;
	}
};

using Vector4d = Vector<4>;

static_assert(sizeof(Vector4d) == (4 * sizeof(float)));
static_assert(sizeof(Vector4d) == (2 * sizeof(uint64_t)));
static_assert(sizeof(Vector4d) == 16);

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif