#pragma once
#include "D3D12/DeviceContext.h"
#include "D3D12/CommandContext.h"
#include "D3D12/SwapChain.h"

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

private:
	struct FrameData {
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		UINT64 fenceValue = 0;
	};

	DeviceContext mContext{};
	CommandContext mCmdContext{};
	SwapChain mSwapChain{};

	FrameData mFrameDatas[DeviceContext::MaxFrameCount]{};

	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW ibView{};

	uint32_t mCurrentFrameIndex = 0;

};