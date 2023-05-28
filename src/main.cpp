#include "gui_application.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main(int, char**)
{
	try
	{
		Application application;
		application.run();
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
