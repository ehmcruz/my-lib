#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <cstdint>
#include <cassert>

#include <my-lib/math.h>
#include <my-lib/math-vector.h>

using namespace Mylib::Math;

int main ()
{
	Vector2f v1(1.0f, 2.0f);

	std::cout << v1 << std::endl;

	return 0;
}