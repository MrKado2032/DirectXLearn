#include "pch.h"
#include "WindowManager.h"
#include "Util/Helper.h"

namespace WindowManager {

	static std::vector<std::unique_ptr<Window>> mWindowList;

	void initialize(uint32_t width, uint32_t height, const char* title) {

		// ウインドウの初期化
		if (!glfwInit()) {
			util::Print("Failed to initialize glfw");
			throw std::runtime_error("Failed to initialize glfw");
		}

		// 今回はDirectX12APIを使用するのでOpenGL使用しないに設定
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		auto window = std::make_unique<Window>();
		if (!window->create(width, height, title)) {
			throw std::runtime_error("Failed to create main window");
		}

		mWindowList.push_back(std::move(window));

	}

	void destroy() {
		if (mWindowList.empty()) return;

		for (auto& window : mWindowList) {
			window->destroy();
		}
		mWindowList.clear();

		glfwTerminate();

	}

	Window* getMainWindow(){
		return mWindowList[0].get();
	}

}