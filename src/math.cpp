#include <iostream>

#include <my-lib/math-matrix.h>
#include <my-lib/math-vector.h>

namespace Mylib
{
namespace Math
{

// ---------------------------------------------------

void Vector2d::println () const
{
	std::cout << "[" << this->x << ", " << this->y << "]" << std::endl;
}

void Vector4d::println () const
{
	std::cout << "[" << this->x << ", " << this->y << ", " << this->z << ", " << this->w << "]" << std::endl;
}

// ---------------------------------------------------

void Matrix4d::println () const
{
	auto& m = *this;
	for (uint32_t i=0; i<get_nrows(); i++) {
		for (uint32_t j=0; j<get_ncols(); j++)
			std::cout << m(i, j) << ", ";
		std::cout << std::endl;
	}
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib
