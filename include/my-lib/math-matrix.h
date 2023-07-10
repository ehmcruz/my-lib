#ifndef __MY_LIBS_MATH_MATRIX_HEADER_H__
#define __MY_LIBS_MATH_MATRIX_HEADER_H__

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/matrix.h>
#include <my-lib/math-vector.h>

namespace Mylib
{
namespace Math
{

// ---------------------------------------------------

class Matrix4d : public StaticMatrix<float, 4, 4>
{
public:
	void set_zero ()
	{
		float *r = this->get_raw();
		for (uint32_t i=0; i<16; i++)
			r[i] = 0.0f;
	}

	void set_identity ()
	{
		float *r = this->get_raw();
		for (uint32_t i=0; i<16; i++)
			r[i] = 0.0f;
		r[0*4 + 0] = 1.0f;
		r[1*4 + 1] = 1.0f;
		r[2*4 + 2] = 1.0f;
		r[3*4 + 3] = 1.0f;
	}

	void set_translate (const Vector2d& v)
	{
		float *r = this->get_raw();
		for (uint32_t i=0; i<16; i++)
			r[i] = 0.0f;
		r[0*4 + 3] = v.x;
		r[1*4 + 3] = v.y;
		r[3*4 + 3] = 1.0f;
	}

	void print ();
};

Matrix4d operator* (const Matrix4d& a_, const Matrix4d& b_);
Vector4d operator* (const Matrix4d& m_, const Vector4d& v_);

inline Vector4d operator* (const Matrix4d& m_, const Vector4d&& v_)
{
	return m_ * v_; // forward b r-value as l-value
}

inline Vector4d operator* (const Matrix4d& m_, const Vector2d& v_)
{
	return m_ * Vector4d(v_);
}


// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif