#ifndef __MY_LIBS_MATRIX_HEADER_H__
#define __MY_LIBS_MATRIX_HEADER_H__

#include <my-lib/std.h>

namespace Mylib
{

// ---------------------------------------------------

template<typename T, int rows, int cols>
class StaticMatrix
{
private:
	T storage[rows * cols];

public:
	inline T* get_raw () const
	{
		return this->storage;
	}

	inline T& operator() (uint32_t row, uint32_t col) const
	{
		return this->storage[row*cols + col];
	}
};

// ---------------------------------------------------

template<typename T>
class Matrix
{
private:
	T *storage;
	OO_ENCAPSULATE_READONLY(uint32_t, rows)
	OO_ENCAPSULATE_READONLY(uint32_t, cols)

public:
	Matrix ()
	{
		this->storage = nullptr;
	}

	Matrix (uint32_t rows, uint32_t cols)
	{
		this->storage = nullptr;
		this->setup(rows, cols);
	}

	~Matrix ()
	{
		if (this->storage != nullptr)
			delete[] this->storage;
	}

	void setup (uint32_t rows, uint32_t cols)
	{
		if (this->storage != nullptr)
			delete[] this->storage;
		this->storage = new T[rows * cols];
		this->rows = rows;
		this->cols = cols;
	}

	inline T* get_raw ()
	{
		return this->storage;
	}

	inline T& get (uint32_t row, uint32_t col)
	{
		return this->storage[row*this->cols + col];
	}

	inline T& operator() (uint32_t row, uint32_t col)
	{
		return this->get(row, col);
	}
};

// ---------------------------------------------------

} // end namespace Mylib

#endif