#ifndef __MY_LIBS_MATH_MATRIX_HEADER_H__
#define __MY_LIBS_MATH_MATRIX_HEADER_H__

#include <concepts>

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/macros.h>
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
	consteval static uint32_t get_dim ()
	{
		return 4;
	}
	
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

	void set_scale (const Vector2d& v)
	{
		float *r = this->get_raw();
		for (uint32_t i=0; i<16; i++)
			r[i] = 0.0f;
		r[0*4 + 0] = v.x;
		r[1*4 + 1] = v.y;
		r[2*4 + 2] = 1.0f;
		r[3*4 + 3] = 1.0f;
	}

	inline void set_scale (const Vector2d&& v)
	{
		this->set_scale(v);
	}

	void set_scale (const float s)
	{
		float *r = this->get_raw();
		for (uint32_t i=0; i<16; i++)
			r[i] = 0.0f;
		r[0*4 + 0] = s;
		r[1*4 + 1] = s;
		r[2*4 + 2] = 1.0f;
		r[3*4 + 3] = 1.0f;
	}

	void set_translate (const Vector2d& v)
	{
		float *r = this->get_raw();

		for (uint32_t i=0; i<16; i++)
			r[i] = 0.0f;

		r[0*4 + 0] = 1.0f;
		r[1*4 + 1] = 1.0f;
		r[2*4 + 2] = 1.0f;

		r[0*4 + 3] = v.x;
		r[1*4 + 3] = v.y;
		r[3*4 + 3] = 1.0f;
	}

	inline void set_translate (const Vector2d&& v)
	{
		this->set_translate(v);
	}

	void println () const;
};

template<typename T>
concept is_matrix_4d = std::same_as<T, Matrix4d> || std::same_as<T, Matrix4d&> || std::same_as<T, Matrix4d&&>;

template<typename T>
concept is_matrix = is_matrix_4d<T>;

template<typename T>
concept is_vector_4d = std::same_as<T, Vector4d> || std::same_as<T, Vector4d&> || std::same_as<T, Vector4d&&>;

template<typename T>
concept is_vector = is_vector_4d<T>;

template <typename Ta, typename Tb>
requires is_matrix<Ta> && is_matrix<Tb>
Matrix4d operator* (Ta&& a_, Tb&& b_)
{
	Matrix4d r_;
	uint32_t i;
	const float *a, *b;
	float *r;
	constexpr uint32_t dim = a_.get_dim();

	static_assert(a_.get_dim() == b_.get_dim());

	a = a_.get_raw();
	b = b_.get_raw();
	r = r_.get_raw();

	for (i=0; i<(dim*dim); i++)
			r[i] = 0.0f;

	for (i=0; i<dim; i++) {
		for (uint32_t k=0; k<dim; k++) {
			const float v = a[i*dim + k];
			for (uint32_t j=0; j<dim; j++)
				r[i*dim + j] += v * b[k*dim + j];
		}
	}
	
	return r_;
}

template <typename Tm, typename Tv>
requires is_matrix<Tm> && is_vector<Tv>
Vector4d operator* (Tm&& m_, Tv&& v_)
{
	Vector4d r_;
	const float *m, *v;
	float *r;
	constexpr uint32_t dim = v_.get_dim();

	static_assert(m_.get_dim() == v_.get_dim());

	m = m_.get_raw();
	v = v_.get_raw();
	r = r_.get_raw();

	for (uint32_t i=0; i<dim; i++) {
		r[i] = 0.0f;
		for (uint32_t j=0; j<dim; j++)
			r[i] += m[i*dim + j] * v[j];
	}
	
	return r_;
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif