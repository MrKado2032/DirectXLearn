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
	mModelList.clear();
	GraphicsCore::destroy();

	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void Application::run()
{
	start();
	while (!glfwWindowShouldClose(mWindow)) {
		glfwPollEvents();

		auto& ctx = mRenderer.begin();	// 描画開始

		update(1);
		render(ctx);

		mRenderer.end();				// 描画終了
	}
}

void Application::createModel()
{
	std::vector<Mesh::VertexData> vertices = {
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}}, // left-top
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}}, // right-top
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // right-down
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // left-down
	};

	std::vector<DWORD> indices = {
		0, 2, 3, 0, 1, 2
	};

	auto mesh = MeshGenerator::generateMesh(vertices, indices);
	auto texture = TextureLoader::loadTextureFromFile(L"Assets/vulkan14.jpg");
	auto material = MaterialGenerator::generateMaterial(texture);
	auto model = ModelGenerator::generateModel(mesh, material);

	mModelList.push_back(std::move(model));
}

void Application::start()
{
	createModel();
}

void Application::update(float dt)
{
	ImGui::Begin("Information");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();

	for (auto const& model : mModelList) {
		//model.transform.update()
	}
}

void Application::render(CommandContext& ctx)
{
	for (auto& model : mModelList) {
		model.draw(ctx);
	}
}

void Application::resize(GLFWwindow* window, int width, int height)
{
	// ウィンドウに紐付けたポインタを取り出し、Application クラスにキャストする
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

	if (app) {
		app->mRenderer.resizeSwapchain(static_cast<UINT>(width), static_cast<UINT>(height));
	}
}
