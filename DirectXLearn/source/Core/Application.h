#pragma once
#include "Renderer.h"

struct GLFWwindow;
class Application {
public:
	Application();
	virtual ~Application();

	Application(const Application&) = delete;
	Application operator=(const Application&) = delete;

	void run();

	void createModel(const std::string& key);

protected:
	virtual void start();
	virtual void update(float dt);

private:
	void render(CommandContext& ctx);
	static void resize(GLFWwindow* window, int width, int height);

	Renderer mRenderer{};

	std::unordered_map<std::string, Model> mModelList;
};

Application* createApplication();