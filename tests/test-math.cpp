#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <cstdint>
#include <cassert>

#include <my-lib/math.h>
#include <my-lib/math-vector.h>
#include <my-lib/math-matrix.h>
#include <my-lib/math-quaternion.h>

using namespace Mylib::Math;
using Matrix2f = Matrix<float, 2, 2>;
using Matrix3f = Matrix<float, 3, 3>;
using Matrix4f = Matrix<float, 4, 4>;

int main ()
{
	std::cout << "----------------------" << std::endl;
	Vector2f v1(2.0f, 3.0f);
	std::cout << v1 << std::endl;

	Matrix4f m;

	std::cout << "----------------------" << std::endl;
	m.set_identity();
	std::cout << m << std::endl;

	std::cout << "----------------------" << std::endl;
	m.set_scale(v1);
	std::cout << m << std::endl;

	std::cout << "----------------------" << std::endl;
	m.set_translate(v1);
	std::cout << m << std::endl;

	std::cout << "---------------------- m2" << std::endl;
	Matrix4f m2 = Matrix4f::identity();
	m2 *= Matrix4f::scale(v1);
	std::cout << m2 << std::endl;

//const float p[] = { 1.0f, 2.0f, 3.0f, 4.0f };
	std::cout << "---------------------- transpose" << std::endl;
	auto mt = Matrix2f({1.0f, 2.0f, 3.0f, 4.0f});
	std::cout << mt << std::endl;
	mt.transpose();
	std::cout << mt << std::endl;

	Quaternionf q1(1.0f, 2.0f, 3.0f, 4.0f);

	return 0;
}