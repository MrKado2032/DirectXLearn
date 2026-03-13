#pragma once
#include "Renderer.h"

class Application {
public:
	Application();
	virtual ~Application();

	Application(const Application&) = delete;
	Application operator=(const Application&) = delete;

	void run();

protected:
	virtual void start();
	virtual void update(float dt);

private:
	GLFWwindow* mWindow = nullptr;
	Renderer mRenderer{};
};