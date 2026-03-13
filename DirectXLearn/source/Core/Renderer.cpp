#include "pch.h"
#include "Renderer.h"

#include "Util/Helper.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::create(uint32_t width, uint32_t height, HWND hWnd)
{;
	try {
		mContext.create();
		mSwapChain.create(mContext, width, height, hWnd);
	}
	catch (std::exception& e) {
		util::Print(e.what());
	}

	// コマンドアロケーターの作成
	auto device = mContext.getDevice();
	for (auto& frame : mFrameDatas) {
		util::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(frame.commandAllocator.GetAddressOf())));
	}

	// コマンドリストの作成 (のちのリファクタリングで変わる可能性あり) 
	util::ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mFrameDatas[0].commandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));
	util::ThrowIfFailed(mCommandList->Close());
}

void Renderer::destroy()
{
	UINT64 completedValue = mContext.flushGPU();	// 今ある実行中の処理をすべて待つ
	for (auto& frame : mFrameDatas) {
		frame.fenceValue = completedValue;
	}

	mSwapChain.destroy();
	mContext.destroy();
}

void Renderer::begin()
{
	auto& frameData = mFrameDatas[mCurrentFrameIndex];	// 現在のフレームデータを取得
	mContext.waitForFence(frameData.fenceValue);		// 同期をとる

	util::ThrowIfFailed(frameData.commandAllocator->Reset());
	util::ThrowIfFailed(mCommandList->Reset(frameData.commandAllocator.Get(), nullptr));

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = mSwapChain.getBackBuffer();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCommandList->ResourceBarrier(1, &barrier);										// バリアで画面に書き込める状態にする

	auto const& handle = mSwapChain.getHandle();
	mCommandList->OMSetRenderTargets(1, &handle.cpuHandle, TRUE, nullptr);			// レンダーターゲットビューのセット

	FLOAT clearColors[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	mCommandList->ClearRenderTargetView(handle.cpuHandle, clearColors, 0, nullptr);	// 画面をクリア
}

void Renderer::end()
{
	auto& frameData = mFrameDatas[mCurrentFrameIndex];

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = mSwapChain.getBackBuffer();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCommandList->ResourceBarrier(1, &barrier);	// バリアを元の状態に戻す

	mCommandList->Close();						// これ以上コマンドを書き込めないように閉じる

	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mContext.getGraphicsQueue()->ExecuteCommandLists(1, cmdLists);	// ため込んだコマンドを実行していく

	frameData.fenceValue = mContext.signalFence();					// 現在の実行状況を値で取得 (同期をとるために必要)

	mSwapChain.present();											// 画面をフリップ

	mCurrentFrameIndex = (mCurrentFrameIndex + 1) % DeviceContext::MaxFrameCount;	// フレームを次に移動
}
