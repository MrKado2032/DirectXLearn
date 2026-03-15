#include "pch.h"
#include "Core/Application.h"

int main() {

	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) {
		return EXIT_FAILURE;
	}

	try {
		Application app;
		app.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}