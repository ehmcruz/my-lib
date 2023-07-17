#ifndef __MY_LIBS_MATH_MATRIX_HEADER_H__
#define __MY_LIBS_MATH_MATRIX_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>

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

template <uint32_t nrows, uint32_t ncols>
class Matrix
{

};

// ---------------------------------------------------

template <>
class Matrix<4, 4>
{
private:
	float data[16];

public:
	consteval static uint32_t get_dim ()
	{
		return 4;
	}

	consteval static uint32_t get_nrows ()
	{
		return get_dim();
	}

	consteval static uint32_t get_ncols ()
	{
		return get_dim();
	}

	inline float* get_raw ()
	{
		return this->data;
	}

	inline const float* get_raw () const
	{
		return this->data;
	}

	inline float& operator() (const uint32_t row, const uint32_t col)
	{
		return this->data[row*get_ncols() + col];
	}

	inline const float operator() (const uint32_t row, const uint32_t col) const
	{
		return this->data[row*get_ncols() + col];
	}
	
	void set_zero ()
	{
		for (uint32_t i=0; i<16; i++)
			this->data[i] = 0.0f;
	}

	void set_identity ()
	{
		for (uint32_t i=0; i<16; i++)
			this->data[i] = 0.0f;
		this->data[0*4 + 0] = 1.0f;
		this->data[1*4 + 1] = 1.0f;
		this->data[2*4 + 2] = 1.0f;
		this->data[3*4 + 3] = 1.0f;
	}

	void set_scale (const Vector2d& v)
	{
		for (uint32_t i=0; i<16; i++)
			this->data[i] = 0.0f;
		this->data[0*4 + 0] = v.x;
		this->data[1*4 + 1] = v.y;
		this->data[2*4 + 2] = 1.0f;
		this->data[3*4 + 3] = 1.0f;
	}

	void set_translate (const Vector2d& v)
	{
		for (uint32_t i=0; i<16; i++)
			this->data[i] = 0.0f;

		this->data[0*4 + 0] = 1.0f;
		this->data[1*4 + 1] = 1.0f;
		this->data[2*4 + 2] = 1.0f;

		this->data[0*4 + 3] = v.x;
		this->data[1*4 + 3] = v.y;
		this->data[3*4 + 3] = 1.0f;
	}
};

using Matrix4d = Matrix<4, 4>;

template<typename T>
concept is_Matrix4d = std::same_as< typename remove_type_qualifiers<T>::type, Matrix4d >;

// ---------------------------------------------------

template<typename T>
concept is_Matrix = is_Matrix4d<T>;

// ---------------------------------------------------

template <typename Ta, typename Tb>
requires is_Matrix<Ta> && is_Matrix<Tb>
auto operator* (const Ta& a_, const Tb& b_)
{
	constexpr uint32_t dim = remove_type_qualifiers<Ta>::type::get_dim();
	Matrix<dim, dim> r_;
	uint32_t i;
	const float *a, *b;
	float *r;

	static_assert(remove_type_qualifiers<Tb>::type::get_dim() == dim);

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

// ---------------------------------------------------

template <typename Tm, typename Tv>
requires is_Matrix<Tm> && is_Vector<Tv>
auto operator* (const Tm& m_, const Tv& v_)
{
	constexpr uint32_t dim = remove_type_qualifiers<Tv>::type::get_dim();
	Vector<dim> r_;
	const float *m, *v;
	float *r;

	static_assert(remove_type_qualifiers<Tm>::type::get_dim() == dim);

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

template<typename T>
requires is_Matrix<T>
void print (const T& m, std::ostream& out=std::cout)
{
	for (uint32_t i=0; i<T::get_nrows(); i++) {
		for (uint32_t j=0; j<T::get_ncols(); j++)
			out << m(i, j) << ", ";
		out << std::endl;
	}
}

template<typename T>
requires is_Matrix<T>
void println (const T& m, std::ostream& out=std::cout)
{
	print(m, out);
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif