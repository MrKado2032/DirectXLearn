#pragma once
#include "D3D12/CommandContext.h"
#include "D3D12/SwapChain.h"
#include "D3D12/D3D12ImGui.h"

class Renderer {
public:
	static constexpr uint32_t MaxFrameCount = 2;

	Renderer();
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer operator=(const Renderer&) = delete;

	void create(uint32_t width, uint32_t height, GLFWwindow* window);
	void destroy();

	CommandContext& begin();
	void end();

	void resizeSwapchain(UINT width, UINT height);

private:
	struct FrameData {
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		UINT64 fenceValue = 0;
	};

	CommandContext mCmdContext{};
	SwapChain mSwapChain{};

	FrameData mFrameDatas[MaxFrameCount]{};

	D3D12ImGui mImGui;

	UINT mCurrentWidth = 0, mCurrentHeight = 0;

	uint32_t mCurrentFrameIndex = 0;

};