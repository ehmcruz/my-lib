#include <iostream>
#include <exception>

#include <cstdlib>

namespace App {

// -------------------------------------------

void main (const int argc, const char **argv)
{
}

// -------------------------------------------

}

// -------------------------------------------

int main (const int argc, const char **argv)
{
	try {
		App::main(argc, argv);
	}
	catch (const std::exception& e) {
		std::cout << "Exception happenned!" << std::endl << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
