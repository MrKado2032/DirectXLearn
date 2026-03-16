#pragma once
#include "Window.h"

namespace WindowManager {

	void initialize(uint32_t width, uint32_t height, const char* title);
	void destroy();

	Window* getMainWindow();

}