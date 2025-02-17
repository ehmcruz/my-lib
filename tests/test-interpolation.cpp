#include <iostream>

#include <my-lib/math-vector.h>
#include <my-lib/interpolation.h>


int main ()
{
	float x;

	Mylib::LinearInterpolator<float, float> interpolator(10.0f, &x, 2.0f, 10.0f);

	std::cout << "x=" << x << std::endl;

	while (interpolator(0.5f)) {
		std::cout << "x=" << x << std::endl;
	}

	return 0;
}