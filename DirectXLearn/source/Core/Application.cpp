#include "pch.h"
#include "Application.h"

#include "Util/Helper.h"

Application::Application()
{
	// ウインドウの初期化
	if (!glfwInit()) {
		util::Print("Failed to initialize glfw");
		throw std::runtime_error("Failed to initialize glfw");
	}

	// 今回はDirectX12APIを使用するのでOpenGL使用しないに設定
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// ウインドウの作成
	mWindow = glfwCreateWindow(1280, 720, "LearnDirectX12", nullptr, nullptr);
	if (!mWindow) {
		util::Print("Failed to create window");
		glfwTerminate();
		throw std::runtime_error("Failed to create window");
	}

	// レンダラーの作成
	mRenderer.create(1280, 720, glfwGetWin32Window(mWindow));
}

Application::~Application()
{
	mRenderer.destroy();

	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void Application::run()
{
	start();
	while (!glfwWindowShouldClose(mWindow)) {
		glfwPollEvents();

		mRenderer.begin();	// 描画開始

		float dt = 1; // 今は仮に１
		update(dt);

		mRenderer.end();	// 描画終了
	}
}

void Application::start()
{
}

void Application::update(float dt)
{
}
