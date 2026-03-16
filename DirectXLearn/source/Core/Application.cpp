#include "pch.h"
#include "Application.h"

#include "WindowManager.h"
#include "D3D12/GraphicsCore.h"
#include "Util/Helper.h"

Application::Application()
{
	// ウインドウの作成
	WindowManager::initialize(1280, 720, "DirectX12Learn");

	// GraphicsCoreの初期化
	GraphicsCore::initialize();

	// レンダラーの作成
	mRenderer.create(1280, 720, WindowManager::getMainWindow()->getWindow());

	// リサイズコールバックをセット
	glfwSetWindowUserPointer(WindowManager::getMainWindow()->getWindow(), this);
	glfwSetFramebufferSizeCallback(WindowManager::getMainWindow()->getWindow(), resize);
}

Application::~Application()
{
	mRenderer.destroy();
	mModelList.clear();
	GraphicsCore::destroy();
	WindowManager::destroy();
}

void Application::run()
{
	auto window = WindowManager::getMainWindow();

	start();
	while (!window->shouldClose()) {
		window->updateEvents();

		auto& ctx = mRenderer.begin();	// 描画開始

		update(1);
		render(ctx);

		mRenderer.end();				// 描画終了
	}
}

void Application::createModel(const std::string& key)
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

	mModelList.emplace(key, std::move(model));
}

void Application::start()
{
}

void Application::update(float dt)
{
	// テスト用（後に消すかリファクタリング）
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
		model.second.draw(ctx);
	}
}

void Application::resize(GLFWwindow* window, int width, int height)
{
	// ウィンドウに紐付けたポインタを取り出し、Application クラスにキャストする
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

	if (app) {
		WindowManager::getMainWindow()->setWindowSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		app->mRenderer.resizeSwapchain(static_cast<UINT>(width), static_cast<UINT>(height));
	}
}
