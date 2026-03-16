#pragma once

extern Application* createApplication();

int main() {

	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) {
		return EXIT_FAILURE;
	}

	try {
		Application* app = createApplication();
		app->run();
		delete app;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}