#ifndef __MY_LIB_MATH_MATRIX_HEADER_H__
#define __MY_LIB_MATH_MATRIX_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>
#include <ostream>

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/math-vector.h>

namespace Mylib
{
namespace Math
{

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
class Matrix
{
private:
	T data[nrows*ncols];
public:
	using Type = T;

	consteval static uint32_t get_nrows ()
	{
		return nrows;
	}

	consteval static uint32_t get_ncols ()
	{
		return ncols;
	}

	consteval static uint32_t get_length ()
	{
		return get_nrows() * get_ncols();
	}

	inline T* get_raw ()
	{
		return this->data;
	}

	inline const T* get_raw () const
	{
		return this->data;
	}

	inline T& operator() (const uint32_t row, const uint32_t col)
	{
		return this->data[row*get_ncols() + col];
	}

	inline const T operator() (const uint32_t row, const uint32_t col) const
	{
		return this->data[row*get_ncols() + col];
	}
	
	void set_zero ()
	{
		for (uint32_t i = 0; i < get_length(); i++)
			this->data[i] = 0;
	}

	void set_identity ()
	{
		static_assert(nrows == ncols);

		this->set_zero();
		
		for (uint32_t i = 0; i < nrows; i++)
			this->data[i*ncols + i] = 1;
	}

	template <uint32_t vector_dim>
	void set_scale (const Vector<T, vector_dim>& v)
	{
		static_assert(nrows == ncols);
		static_assert(vector_dim <= nrows);

		this->set_zero();
		
		for (uint32_t i = 0; i < vector_dim; i++)
			this->data[i*ncols + i] = v[i];
		
		for (uint32_t i = vector_dim; i < nrows; i++)
			this->data[i*ncols + i] = 1;
	}

	template <uint32_t vector_dim>
	void set_translate (const Vector<T, vector_dim>& v)
	{
		static_assert(nrows == ncols);
		static_assert(vector_dim < nrows);

		this->set_identity();
		
		constexpr uint32_t last = ncols - 1;

		for (uint32_t i = 0; i < vector_dim; i++)
			this->data[i*ncols + last] = v[i];
	}
};

using Matrix4f = Matrix<float, 4, 4>;

template<typename T>
concept is_Matrix4f = std::same_as< typename remove_type_qualifiers<T>::type, Matrix4f >;

// ---------------------------------------------------

template<typename T>
concept is_Matrix = is_Matrix4f<T>;

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
Matrix<T, nrows, ncols> gen_zero_matrix ()
{
	Matrix<T, nrows, ncols> m;
	m.set_zero();
	return m;
}

template <typename T, uint32_t dim>
Matrix<T, dim, dim> gen_identity_matrix ()
{
	Matrix<T, dim, dim> m;
	m.set_identity();
	return m;
}

// ---------------------------------------------------

template <typename Ta, typename Tb>
requires is_Matrix<Ta> && is_Matrix<Tb> && std::same_as<typename Ta::Type, typename Tb::Type>
auto operator* (const Ta& a_, const Tb& b_)
{
	using Type = typename Ta::Type;
	
	// only square matrices for now
	static_assert(Ta::get_nrows() == Ta::get_ncols());
	static_assert(Tb::get_nrows() == Tb::get_ncols());
	static_assert(Ta::get_nrows() == Tb::get_nrows());

	constexpr uint32_t dim = remove_type_qualifiers<Ta>::type::get_ncols();
	
	Matrix<Type, dim, dim> r_;
	const Type *a, *b;
	Type *r;

	a = a_.get_raw();
	b = b_.get_raw();
	r = r_.get_raw();

	for (uint32_t i = 0; i < (dim*dim); i++)
			r[i] = 0;

	for (uint32_t i = 0; i < dim; i++) {
		for (uint32_t k = 0; k < dim; k++) {
			const float v = a[i*dim + k];
			for (uint32_t j = 0; j < dim; j++)
				r[i*dim + j] += v * b[k*dim + j];
		}
	}
	
	return r_;
}

// ---------------------------------------------------

template <typename Tm, typename Tv>
requires is_Matrix<Tm> && is_Vector<Tv> && std::same_as<typename Tm::Type, typename Tv::Type>
auto operator* (const Tm& m_, const Tv& v_)
{
	using Type = typename Tm::Type;

	// only square matrices for now
	static_assert(Tm::get_nrows() == Tm::get_ncols());

	static_assert(Tm::get_nrows() == Tv::get_dim());

	constexpr uint32_t dim = remove_type_qualifiers<Tv>::type::get_dim();

	Vector<Type, dim> r_;
	const Type *m, *v;
	Type *r;

	m = m_.get_raw();
	v = v_.get_raw();
	r = r_.get_raw();

	for (uint32_t i = 0; i < dim; i++) {
		r[i] = 0;
		for (uint32_t j = 0; j < dim; j++)
			r[i] += m[i*dim + j] * v[j];
	}
	
	return r_;
}

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
std::ostream& operator << (std::ostream& out, const Matrix<T, nrows, ncols>& m)
{
	for (uint32_t i = 0; i < nrows; i++) {
		for (uint32_t j = 0; j < ncols; j++)
			out << m(i, j) << ", ";
		out << std::endl;
	}

	return out;
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif