#include "pch.h"
#include "Util/Helper.h"

#include "D3D12/DeviceContext.h"

int main() {

	// ウインドウの初期化
	if (!glfwInit()) {
		util::Print("Failed to initialize glfw");
		return EXIT_FAILURE;
	}

	// 今回はDirectX12APIを使用するのでOpenGL使用しないに設定
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// ウインドウの作成
	GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnDirectX12", nullptr, nullptr);
	if (!window) {
		util::Print("Failed to create window");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// --- ここにDirectX12オブジェクトを作っていく ---

	DeviceContext deviceContext;
	try {
		deviceContext.create();
	}
	catch (std::exception& e) {
		util::Print(e.what());
	}

	// ---

	// メインループ
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	// 終了処理
	deviceContext.destroy();

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}