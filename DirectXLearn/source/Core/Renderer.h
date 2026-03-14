#pragma once
#include "D3D12/DeviceContext.h"
#include "D3D12/CommandContext.h"
#include "D3D12/SwapChain.h"
#include "D3D12/Model.h"
#include "D3D12/ShaderLoader.h"

struct Model;
class Renderer {
public:
	Renderer();
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer operator=(const Renderer&) = delete;

	void create(uint32_t width, uint32_t height, HWND hWnd);
	void destroy();

	void begin();
	void end();

	void drawModel(Model& model);

private:
	struct FrameData {
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		UINT64 fenceValue = 0;
	};

	CommandContext mCmdContext{};
	SwapChain mSwapChain{};

	FrameData mFrameDatas[DeviceContext::MaxFrameCount]{};

	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;

	uint32_t mCurrentFrameIndex = 0;

	ShaderLoader shaderLoader;
	Model mModel;

};