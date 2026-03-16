#pragma once
#include "D3D12/DeviceContext.h"
#include "D3D12/CommandContext.h"
#include "D3D12/SwapChain.h"
#include "D3D12/Model.h"
#include "D3D12/ShaderLoader.h"
#include "D3D12/D3D12ImGui.h"

struct Model;
class Renderer {
public:
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

	FrameData mFrameDatas[DeviceContext::MaxFrameCount]{};

	D3D12ImGui mImGui;

	UINT mCurrentWidth = 0, mCurrentHeight = 0;

	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;

	uint32_t mCurrentFrameIndex = 0;

	ShaderLoader shaderLoader;

};