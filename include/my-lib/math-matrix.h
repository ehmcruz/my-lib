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

	inline T* get_raw () noexcept
	{
		return this->data;
	}

	inline const T* get_raw () const noexcept
	{
		return this->data;
	}

	constexpr T& operator() (const uint32_t i) noexcept
	{
		return this->data[i];
	}

	constexpr const T operator() (const uint32_t i) const noexcept
	{
		return this->data[i];
	}

	constexpr T& operator() (const uint32_t row, const uint32_t col) noexcept
	{
		return this->data[row*get_ncols() + col];
	}

	constexpr const T operator() (const uint32_t row, const uint32_t col) const noexcept
	{
		return this->data[row*get_ncols() + col];
	}

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Matrix& operator OP (const Matrix& other) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP other.data[i]; \
			return *this; \
		} \
		constexpr Matrix& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Matrix& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	constexpr void set_zero () noexcept
	{
		for (uint32_t i = 0; i < get_length(); i++)
			this->data[i] = 0;
	}

	constexpr void set_identity () noexcept
	{
		static_assert(nrows == ncols);

		this->set_zero();
		
		for (uint32_t i = 0; i < nrows; i++)
			this->data[i*ncols + i] = 1;
	}

	template <uint32_t vector_dim>
	constexpr void set_scale (const Vector<T, vector_dim>& v) noexcept
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
	constexpr void set_translate (const Vector<T, vector_dim>& v) noexcept
	{
		static_assert(nrows == ncols);
		static_assert(vector_dim < nrows);

		this->set_identity();
		
		constexpr uint32_t last = ncols - 1;

		for (uint32_t i = 0; i < vector_dim; i++)
			this->data[i*ncols + last] = v[i];
	}

	constexpr void set_rotation_matrix (const Vector<T, 3>& axis_, const T angle) noexcept
		requires (nrows == ncols && ncols == 3)
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

		w(1, 0) = axis.z;
		w(1, 1) = 0;
		w(1, 2) = -axis.x;

		w(2, 0) = -axis.y;
		w(2, 1) = axis.x;
		w(2, 2) = 0;

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

		m(1, 0) = t*x*y + s*z;
		m(1, 1) = t*y*y + c;
		m(1, 2) = t*y*z - s*x;

		m(2, 0) = t*x*z - s*y;
		m(2, 1) = t*y*z + s*x;
		m(2, 2) = t*z*z + c;
	#endif
	}

	constexpr void set_rotation_matrix (const Vector<T, 3>& axis_, const T angle) noexcept
		requires (nrows == ncols && ncols == 4)
	{
		Matrix<T, 3, 3> m;

		m.set_rotation_matrix(axis_, angle);

		for (uint32_t i = 0; i < 3; i++) {
			for (uint32_t j = 0; j < 3; j++)
				this->data[i*ncols + j] = m(i, j);
		}

		(*this)(0, 3) = 0;
		(*this)(1, 3) = 0;
		(*this)(2, 3) = 0;

		(*this)(3, 0) = 0;
		(*this)(3, 1) = 0;
		(*this)(3, 2) = 0;

		(*this)(3, 3) = 1;
	}

	constexpr void set_perspective_matrix (const T fovy,
	                                       const T screen_width,
										   const T screen_height,
										   const T znear,
										   const T zfar,
										   const T handedness
										   ) noexcept
		requires (nrows == ncols && ncols == 4)
	{
		Matrix<T, 4, 4> m;

		const T aspect = screen_width / screen_height;

		auto& m = *this;

		const T zdist = (znear - zfar);

		m(0, 0) = fp(1) / (aspect * std::tan(fovy * fp(0.5)));
		m(0, 1) = 0;
		m(0, 2) = 0;
		m(0, 3) = 0;

		m(1, 0) = 0;
		m(1, 1) = fp(1) / std::tan(fovy * fp(0.5));
		m(1, 2) = 0;
		m(1, 3) = 0;

		m(2, 0) = 0;
		m(2, 1) = 0;
		m(2, 2) = zfar / zdist * handedness;

/* const T y = 1 / std::tan(fovy * static_cast<T>(.5));
  const T x = y / aspect;
  const T zdist = (znear - zfar);
  const T zfar_per_zdist = zfar / zdist;
  return Matrix<T, 4, 4>
  (x, 0, 0, 0, 
   0, y, 0, 0,
   0, 0, zfar_per_zdist * handedness, -1 * handedness,
   0, 0, 2.0f * znear * zfar_per_zdist, 0);*/

	}
};

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
Matrix<T, nrows, ncols> gen_zero_matrix () noexcept
{
	Matrix<T, nrows, ncols> m;
	m.set_zero();
	return m;
}

template <typename T, uint32_t dim>
Matrix<T, dim, dim> gen_identity_matrix () noexcept
{
	Matrix<T, dim, dim> m;
	m.set_identity();
	return m;
}

// ---------------------------------------------------

template <typename T, uint32_t nrows_a, uint32_t ncols_a, uint32_t ncols_b>
constexpr Matrix<T, nrows_a, ncols_b> operator* (const Matrix<T, nrows_a, ncols_a>& a,
                                                 const Matrix<T, ncols_a, ncols_b>& b) noexcept
{
	Matrix<T, nrows_a, ncols_b> r;

	r.set_zero();

	for (uint32_t i = 0; i < nrows_a; i++) {
		for (uint32_t k = 0; k < ncols_a; k++) {
			const T v = a(i, k);

			for (uint32_t j = 0; j < ncols_b; j++)
				r(i, j) += v * b(k, j);
		}
	}
	
	return r;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
Vector<T, dim> operator* (const Matrix<T, dim, dim>& m, const Vector<T, dim>& v) noexcept
{
	Vector<T, dim> r;

	for (uint32_t i = 0; i < dim; i++) {
		r[i] = 0;
		for (uint32_t j = 0; j < dim; j++)
			r[i] += m(i, j) * v[j];
	}
	
	return r;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t nrows, uint32_t ncols> \
	Matrix<T, nrows, ncols> operator OP (const Matrix<T, nrows, ncols>& ma, const Matrix<T, nrows, ncols>& mb) noexcept \
	{ \
		Matrix<T, nrows, ncols> r; \
		for (uint32_t i = 0; i < (nrows*ncols); i++) \
			r(i) = ma(i) OP mb(i); \
		return r; \
	} \
	template <typename T, uint32_t nrows, uint32_t ncols> \
	Matrix<T, nrows, ncols> operator OP (const Matrix<T, nrows, ncols>& ma, const T v) noexcept \
	{ \
		Matrix<T, nrows, ncols> r; \
		for (uint32_t i = 0; i < (nrows*ncols); i++) \
			r(i) = ma(i) OP v; \
		return r; \
	}
	
MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t nrows, uint32_t ncols> \
	Matrix<T, nrows, ncols> operator OP (const Matrix<T, nrows, ncols>& m, const T v) noexcept \
	{ \
		Matrix<T, nrows, ncols> r; \
		for (uint32_t i = 0; i < (nrows*ncols); i++) \
			r(i) = m(i) OP v; \
		return r; \
	}
	
MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

// ---------------------------------------------------

//template <typename T, uint32_t dim>
//Matrix<T, dim, dim> gen_rotation_matrix (const Vector<T, dim>& axis, const T angle) noexcept;

template <typename T, uint32_t dim>
	requires (dim >= 2 && dim <= 3)
Matrix<T, dim, dim> gen_rotation_matrix (const Vector<T, dim>& axis, const T angle) noexcept
{
	Matrix<T, dim, dim> m;
	m.set_rotation_matrix(axis, angle);
	return m;
}

template <typename T>
Matrix<T, 4, 4> gen_rotation_matrix4 (const Vector<T, 3>& axis, const T angle) noexcept
{
	Matrix<T, 4, 4> m;
	m.set_rotation_matrix(axis, angle);
	return m;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
Point<T, dim> rotate_around_vector (const Point<T, dim>& point, const Vector<T, dim>& axis, const T angle) noexcept
	requires (dim >= 2 && dim <= 3)
{
	Point<T, dim> rotated;
	rotated = gen_rotation_matrix<T, dim>(axis, angle) * point;
	return rotated;
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