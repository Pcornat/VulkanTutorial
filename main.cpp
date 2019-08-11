#include <iostream>
#include "hello_triangle_app.h"

int main() {
	try {
		HelloTriangleApp coucou("Hello", 1280, 720);
		coucou.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}