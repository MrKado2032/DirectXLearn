#pragma once
#include "DescriptorAllocator.h"

using Microsoft::WRL::ComPtr;

class DeviceContext;

class SwapChain {
public:
	SwapChain();
	~SwapChain();

	SwapChain(const SwapChain&) = delete;
	SwapChain operator=(const SwapChain&) = delete;

	void create(const DeviceContext& context, uint32_t w, uint32_t h, HWND hWnd);
	void destroy();

	void present();
	void resize(UINT width, UINT height);

	ID3D12Resource* getBackBuffer() const {
		auto imageIdx = mSwapchain->GetCurrentBackBufferIndex();
		return mFrameResources[imageIdx].backBuffer.Get();
	}

	DescriptorHandle getHandle() const {
		auto imageIdx = mSwapchain->GetCurrentBackBufferIndex();
		return mFrameResources[imageIdx].handle;
	}

private:
	struct FrameResource {
		ComPtr<ID3D12Resource> backBuffer;
		DescriptorHandle handle;
	};

	ComPtr<IDXGISwapChain4> mSwapchain;
	DescriptorAllocator mRtvDescriptorAllocator;
	std::vector<FrameResource> mFrameResources;
};