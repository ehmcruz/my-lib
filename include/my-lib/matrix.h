#ifndef __MY_LIBS_MATRIX_HEADER_H__
#define __MY_LIBS_MATRIX_HEADER_H__

#include <cstdlib>
#include <cstring>

#include <my-lib/std.h>

namespace Mylib
{

// ---------------------------------------------------

template<typename T, uint32_t nrows, uint32_t ncols>
class StaticMatrix
{
private:
	T storage[nrows * ncols];

	void copy (const StaticMatrix& other)
	{
		for (uint32_t i=0; i<(nrows*ncols); i++)
			this->storage[i] = other.storage[i];
	}

public:
	StaticMatrix () = default;

	// in copy constructor/assign, we will perform a deep-copy
	// in move constructor/assign, we will perform a shallow-copy

	// copy-constructor
	StaticMatrix (const StaticMatrix& other)
	{
		this->copy(other); // perform deep-copy
	}

	// move constructor
	StaticMatrix (StaticMatrix&& other)
	{
		// shallow-copy
		memcpy(this->storage, other.storage, nrows * ncols * sizeof(T));
	}

	// copy-assign operator
	StaticMatrix& operator= (const StaticMatrix& other)
	{
		this->copy(other); // perform deep-copy
		return *this;
	}

	// move-assign operator
	StaticMatrix& operator= (StaticMatrix&& other)
	{
		// shallow-copy
		memcpy(this->storage, other.storage, nrows * ncols * sizeof(T));
		return *this;
	}

	consteval static uint32_t get_nrows ()
	{
		return nrows;
	}

	consteval static uint32_t get_ncols ()
	{
		return ncols;
	}

	inline T* get_raw ()
	{
		return this->storage;
	}

	inline const T* get_raw () const
	{
		return this->storage;
	}

/*	inline const T* get_raw_const () const
	{
		return this->storage;
	}*/

	inline T& operator() (const uint32_t row, const uint32_t col)
	{
		return this->storage[row*ncols + col];
	}

	inline const T& operator() (const uint32_t row, const uint32_t col) const
	{
		return this->storage[row*ncols + col];
	}
};

// ---------------------------------------------------

template<typename T>
class Matrix
{
private:
	T *storage;
	OO_ENCAPSULATE_READONLY(uint32_t, nrows)
	OO_ENCAPSULATE_READONLY(uint32_t, ncols)

	void alloc (const uint32_t nrows, const uint32_t ncols)
	{
		if (this->storage != nullptr && (this->nrows != nrows || this->ncols != ncols))
			delete[] this->storage;
		this->storage = new T[nrows * ncols];
		this->nrows = nrows;
		this->ncols = ncols;
	}

	void copy (const Matrix& other)
	{
		this->alloc(other.nrows, other.ncols);
		for (uint32_t i=0; i<(this->nrows * this->ncols); i++)
			this->storage[i] = other.storage[i];
	}

	void move (Matrix&& other)
	{
		if (this->storage != nullptr)
			delete[] this->storage;
		this->nrows = other.nrows;
		this->ncols = other.ncols;
		this->storage = other.storage;
		other.storage = nullptr;
	}

public:
	Matrix ()
	{
		this->storage = nullptr;
	}

	Matrix (const uint32_t nrows_, const uint32_t ncols_)
	{
		this->storage = nullptr;
		this->alloc(nrows_, ncols_);
	}

	~Matrix ()
	{
		if (this->storage != nullptr)
			delete[] this->storage;
	}

	// copy-constructor
	Matrix (const Matrix& other)
	{
		this->storage = nullptr;
		this->copy(other);
	}

	// move constructor
	Matrix (Matrix&& other)
	{
		this->storage = nullptr;
		this->move( std::move(other) );
	}

	// copy-assign operator
	Matrix& operator= (const Matrix& other)
	{
		this->copy(other); // perform deep-copy
		return *this;
	}

	// move-assign operator
	Matrix& operator= (Matrix&& other)
	{
		this->move( std::move(other) );
		return *this;
	}

	inline T* get_raw ()
	{
		return this->storage;
	}

	inline const T* get_raw_const () const
	{
		return this->storage;
	}

	inline T& operator() (const uint32_t row, const uint32_t col)
	{
		return this->storage[row*this->ncols + col];
	}

	inline T& operator() (const uint32_t row, const uint32_t col) const
	{
		return this->storage[row*this->ncols + col];
	}
};

// ---------------------------------------------------

} // end namespace Mylib

#endif