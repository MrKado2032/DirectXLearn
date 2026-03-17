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
		updateKey();

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

	mCamera.update();

	for (auto const& model : mModelList) {
		model.second.transform.update(mCamera.getViewProjectionMatrix());
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

void Application::updateKey()
{
	auto window = WindowManager::getMainWindow()->getWindow();
	ImGuiIO& io = ImGui::GetIO();

	// --- 1. マウス状態の更新 ---
	bool isRightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	if (isRightPressed) {
		if (!bMouseRightToggle) {
			bMouseRightToggle = true;
			bFirstMouse = true; // 次のフレームで移動量をリセット
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	else {
		if (bMouseRightToggle) {
			bMouseRightToggle = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	// --- 2. 視点回転処理 (右クリック中のみ) ---
	if (bMouseRightToggle) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		if (bFirstMouse) {
			lastX = xpos;
			lastY = ypos;
			bFirstMouse = false;
		}

		float dx = static_cast<float>(xpos - lastX);
		float dy = static_cast<float>(ypos - lastY);
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.002f;
		mCamera.rotate(dy * sensitivity, dx * sensitivity);
	}

	// --- 3. 移動処理 (ImGui操作中でない場合のみ) ---
	// 右クリック操作中は、ImGuiのウィンドウ上であっても移動を優先する
	if (!bMouseRightToggle) {
		if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
			return; // ImGuiがマウスを欲しがっているときは以下の移動処理をスキップ
		}
	}

	float moveSpeed = 0.1f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) mCamera.moveForward(moveSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) mCamera.moveForward(-moveSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) mCamera.moveRight(moveSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) mCamera.moveRight(-moveSpeed);
}
