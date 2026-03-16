#pragma once

class Window {
public:
	Window();
	~Window();

	Window(const Window&) = delete;
	Window operator=(const Window&) = delete;

	bool create(uint32_t width, uint32_t height, const char* title);
	void destroy();

	bool shouldClose() const;
	void updateEvents();

	void setWindowSize(uint32_t width, uint32_t height);

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	GLFWwindow* getWindow() const { return window; }

private:
	GLFWwindow* window = nullptr;
	uint32_t mWidth = 0, mHeight = 0;
};