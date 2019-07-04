#include <iostream>
#include "hello_triangle_app.h"

int main() {
	try {
		HelloTriangleApp().run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}