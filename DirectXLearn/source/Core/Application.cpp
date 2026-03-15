#include "pch.h"
#include "Application.h"

#include "D3D12/GraphicsCore.h"
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

	// GraphicsCoreの初期化
	GraphicsCore::initialize();

	// レンダラーの作成
	mRenderer.create(1280, 720, mWindow);

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetFramebufferSizeCallback(mWindow, resize);
}

Application::~Application()
{
	mRenderer.destroy();
	GraphicsCore::destroy();

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
	ImGui::Begin("Information");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void Application::resize(GLFWwindow* window, int width, int height)
{
	// ウィンドウに紐付けたポインタを取り出し、Application クラスにキャストする
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

	if (app) {
		app->mRenderer.resizeSwapchain(static_cast<UINT>(width), static_cast<UINT>(height));
	}
}
