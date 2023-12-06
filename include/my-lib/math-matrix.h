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

#ifdef MYLIB_MATH_BUILD_OPERATION
#error nooooooooooo
#endif

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

	consteval static T fp (const auto v)
	{
		return static_cast<T>(v);
	}

	inline T* get_raw ()
	{
		return this->data;
	}

	inline const T* get_raw () const
	{
		return this->data;
	}

	constexpr T& operator() (const uint32_t i)
	{
		return this->data[i];
	}

	constexpr const T operator() (const uint32_t i) const
	{
		return this->data[i];
	}

	constexpr T& operator() (const uint32_t row, const uint32_t col)
	{
		return this->data[row*get_ncols() + col];
	}

	constexpr const T operator() (const uint32_t row, const uint32_t col) const
	{
		return this->data[row*get_ncols() + col];
	}

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		inline Matrix& operator OP (const Matrix& other) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP other.data[i]; \
			return *this; \
		} \
		inline Matrix& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )

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

	void set_rotation_matrix (const Vector<T, 3>& axis_, const T angle) noexcept
		requires (nrows == 4 && ncols == 4)
	{
		const T c = std::cos(angle);
		const T s = std::sin(angle);
		
		const T t = fp(1) - c;
		//const T sh = std::sin(angle / fp(2));
		//const T t = fp(2) * sh * sh;

		const Vector<T, 3> axis = normalize(axis_);

		// Rodrigues' rotation

	#if 1
		Matrix w;

		w(0, 0) = 0;
		w(0, 1) = -axis.z;
		w(0, 2) = axis.y;
		w(0, 3) = 0;

		w(1, 0) = axis.z;
		w(1, 1) = 0;
		w(1, 2) = -axis.x;
		w(1, 3) = 0;

		w(2, 0) = -axis.y;
		w(2, 1) = axis.x;
		w(2, 2) = 0;
		w(2, 3) = 0;

		w(3, 0) = 0;
		w(3, 1) = 0;
		w(3, 2) = 0;
		w(3, 3) = 1;

		const Matrix w2 = w * w;

		this->set_identity();
		*this += w * s + w2 * t;
	#else
		const T x = axis.x;
		const T y = axis.y;
		const T z = axis.z;

		auto& m = *this;

		m(0, 0) = t*x*x + c;
		m(0, 1) = t*x*y - s*z;
		m(0, 2) = t*x*z + s*y;
		m(0, 3) = 0;

		m(1, 0) = t*x*y + s*z;
		m(1, 1) = t*y*y + c;
		m(1, 2) = t*y*z - s*x;
		m(1, 3) = 0;

		m(2, 0) = t*x*z - s*y;
		m(2, 1) = t*y*z + s*x;
		m(2, 2) = t*z*z + c;
		m(2, 3) = 0;

		m(3, 0) = 0;
		m(3, 1) = 0;
		m(3, 2) = 0;
		m(3, 3) = 1;
	#endif
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
Matrix<T, nrows, ncols> operator+ (const Matrix<T, nrows, ncols>& ma, const Matrix<T, nrows, ncols>& mb)
{
	Matrix<T, nrows, ncols> r;

	for (uint32_t i = 0; i < (nrows*ncols); i++)
		r(i) = ma(i) + mb(i);

	return r;
}

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
Matrix<T, nrows, ncols> operator* (const Matrix<T, nrows, ncols>& m, const T v)
{
	Matrix<T, nrows, ncols> r;

	for (uint32_t i = 0; i < (nrows*ncols); i++)
		r(i) = m(i) * v;

	return r;
}

// ---------------------------------------------------

template <typename T>
Matrix<T, 4, 4> gen_rotation_matrix (const Vector<T, 3>& axis, const T angle)
{
	Matrix<T, 4, 4> m;
	m.set_rotation_matrix(axis, angle);
	return m;
}

// ---------------------------------------------------

template <typename T>
Point<T, 3> rotate_around_vector (const Point<T, 3>& point, const Vector<T, 3>& axis, const T angle)
{
	Point<T, 4> rotated;
	Point<T, 4> point4(point.x, point.y, point.z, 1);
	rotated = gen_rotation_matrix(axis, angle) * point4;
	return Point<T, 3>(rotated.x, rotated.y, rotated.z);
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

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif