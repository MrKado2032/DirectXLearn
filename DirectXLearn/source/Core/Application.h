#pragma once
#include "Renderer.h"
#include "D3D12/Camera.h"
#include "D3D12/Model.h"

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
	void updateKey();

	Renderer mRenderer{};
	Camera mCamera{ 1280, 720 };
	double lastX = 0.0;
	double lastY = 0.0;
	bool bMouseRightToggle = false;
	bool bFirstMouse = true;

	std::unordered_map<std::string, Model> mModelList;
};

Application* createApplication();