#include "pch.h"
#include "SwapChain.h"

#include "Util/Helper.h"
#include "DeviceContext.h"

SwapChain::SwapChain()
{
}

SwapChain::~SwapChain()
{
}

void SwapChain::create(const DeviceContext& context, uint32_t w, uint32_t h, HWND hWnd)
{
	auto device = context.getDevice();
	auto factory = context.getFactory();
	auto gQueue = context.getGraphicsQueue();

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = static_cast<UINT>(w);
	swapchainDesc.Height = static_cast<UINT>(h);
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.BufferCount = static_cast<UINT>(DeviceContext::MaxFrameCount);
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// レンダーターゲットとして使用
	swapchainDesc.Flags = 0;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.Stereo = FALSE;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// スワップチェインがフリップするたびに前のコマンドデータを破棄するみたいなやつ

	ComPtr<IDXGISwapChain1> swapchain1;
	util::ThrowIfFailed(factory->CreateSwapChainForHwnd(gQueue, hWnd, &swapchainDesc, nullptr, nullptr, swapchain1.GetAddressOf()));
	util::ThrowIfFailed(swapchain1.As(&mSwapchain));

	util::ThrowIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));	// ALT + ENTER 操作をしない

	mRtvDescriptorAllocator.create(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, DeviceContext::MaxFrameCount + 1, false);	// ダブルバッファリングだが余裕をもって3つ分確保

	// レンダーターゲットビューの作成
	mFrameResources.resize(DeviceContext::MaxFrameCount);
	for (UINT i = 0; i < DeviceContext::MaxFrameCount; i++) {
		util::ThrowIfFailed(mSwapchain->GetBuffer(i, IID_PPV_ARGS(mFrameResources[i].backBuffer.GetAddressOf())));
		mFrameResources[i].handle = mRtvDescriptorAllocator.allocate();
		device->CreateRenderTargetView(mFrameResources[i].backBuffer.Get(), nullptr, mFrameResources[i].handle.cpuHandle);
	}
}

void SwapChain::destroy()
{
	for (auto& resource : mFrameResources) {
		resource.backBuffer.Reset();
		mRtvDescriptorAllocator.free(resource.handle);
	}
	mFrameResources.clear();

	mRtvDescriptorAllocator.destroy();
	if (mSwapchain) {
		mSwapchain.Reset();
	}
}

void SwapChain::present()
{
	mSwapchain->Present(1, 0);
}
