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
	
		// コマンドアロケーターの作成
		auto device = mContext.getDevice();
		for (auto& frame : mFrameDatas) {
			util::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(frame.commandAllocator.GetAddressOf())));
		}

		mCmdContext.create(mContext.getDevice(), mFrameDatas[0].commandAllocator.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
	catch (std::exception& e) {
		util::Print(e.what());
	}
}

void Renderer::destroy()
{
	// 今ある実行中の処理をすべて待つ
	UINT64 completedValue = mContext.flushGPU();
	for (auto& frame : mFrameDatas) {
		frame.fenceValue = completedValue;
	}

	mSwapChain.destroy();
	mCmdContext.destroy();
	mContext.destroy();
}

void Renderer::begin()
{
	auto& frameData = mFrameDatas[mCurrentFrameIndex];	// 現在のフレームデータを取得
	mContext.waitForFence(frameData.fenceValue);		// 同期をとる

	util::ThrowIfFailed(frameData.commandAllocator->Reset());
	
	// コマンド書き込み開始
	mCmdContext.begin(frameData.commandAllocator.Get());

	// バリアで画面に書き込める状態にする
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// 画面をクリア
	auto handle = mSwapChain.getHandle();
	FLOAT clearColors[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	mCmdContext.clearRenderTarget(handle.cpuHandle, clearColors);
}

void Renderer::end()
{
	auto& frameData = mFrameDatas[mCurrentFrameIndex];

	// バリアを元の状態に戻す
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// コマンドの書き込みを終了
	mCmdContext.end();

	// ため込んだコマンドを実行していく
	ID3D12CommandList* cmdLists[] = { mCmdContext.getCommandLists() };
	mContext.getGraphicsQueue()->ExecuteCommandLists(1, cmdLists);

	// 現在の実行状況を値で取得 (同期をとるために必要)
	frameData.fenceValue = mContext.signalFence();

	// 画面をフリップ
	mSwapChain.present();

	// フレームを次に移動
	mCurrentFrameIndex = (mCurrentFrameIndex + 1) % DeviceContext::MaxFrameCount;
}
