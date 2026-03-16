#include "pch.h"
#include "Window.h"

#include "Util/Helper.h"

Window::Window()
{
}

Window::~Window()
{
}

bool Window::create(uint32_t width, uint32_t height, const char* title)
{
	// ウインドウの作成
	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);
	if (!window) {
		util::Print("Failed to create window");
		return false;
	}

	mWidth = width;
	mHeight = height;

	return true;
}

void Window::destroy()
{
	glfwDestroyWindow(window);
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

void Window::updateEvents()
{
	glfwPollEvents();
}

void Window::setWindowSize(uint32_t width, uint32_t height)
{
	mWidth = width;
	mHeight = height;
}
