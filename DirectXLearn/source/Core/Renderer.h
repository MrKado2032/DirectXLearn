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

	uint32_t mCurrentFrameIndex = 0;

};