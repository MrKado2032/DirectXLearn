#include "pch.h"
#include "Renderer.h"

#include "Util/Helper.h"

#include "D3D12/GraphicsCore.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::create(uint32_t width, uint32_t height, GLFWwindow* window)
{
	try {
		auto const& context = GraphicsCore::getDeviceContext();

		// スワップチェインの作成
		mSwapChain.create(MaxFrameCount, width, height, glfwGetWin32Window(window));
	
		// コマンドアロケーターの作成
		auto device = context.getDevice();
		for (auto& frame : mFrameDatas) {
			util::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(frame.commandAllocator.GetAddressOf())));
		}

		// コマンドコンテキストの作成
		mCmdContext.create(device, mFrameDatas[0].commandAllocator.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

		mCurrentWidth = width;
		mCurrentHeight = height;

		// ImGuiの初期化
		mImGui.initialize(window, MaxFrameCount);
	}
	catch (std::exception& e) {
		util::Print(e.what());
	}
}

void Renderer::destroy()
{
	// 今ある実行中の処理をすべて待つ
	auto& context = GraphicsCore::getDeviceContext();
	UINT64 completedValue = context.flushGPU();
	for (auto& frame : mFrameDatas) {
		frame.fenceValue = completedValue;
	}

	mImGui.destroy();

	mSwapChain.destroy();
	mCmdContext.destroy();
}

CommandContext& Renderer::begin()
{
	auto& context = GraphicsCore::getDeviceContext();
	auto& rsMgr = GraphicsCore::getRootSignatureManager();
	auto& psoMgr = GraphicsCore::getPSOManager();

	auto& frameData = mFrameDatas[mCurrentFrameIndex];	// 現在のフレームデータを取得
	context.waitForFence(frameData.fenceValue);			// 同期をとる

	util::ThrowIfFailed(frameData.commandAllocator->Reset());
	
	// コマンド書き込み開始
	mCmdContext.begin(frameData.commandAllocator.Get(), psoMgr.findPSOAndGet(PIPELINE_KEY_2DBASE));

	// ルートシグネチャのセット
	mCmdContext.setRootSignature(rsMgr.getBaseRootSignature());

	// SRVヒープのバインド
	auto& heap = GraphicsCore::getSrvCbvUavAllocator();
	mCmdContext.setDescriptorHeaps(heap.getDescriptorHeap());

	// ビューポート
	CD3DX12_VIEWPORT viewport({ 0, 0, static_cast<FLOAT>(mCurrentWidth), static_cast<FLOAT>(mCurrentHeight) });
	mCmdContext.setViewPort(viewport);

	CD3DX12_RECT scissorRect{ 0, 0, static_cast<LONG>(mCurrentWidth), static_cast<LONG>(mCurrentHeight) };
	mCmdContext.setScissorRect(scissorRect);

	// バリアで画面に書き込める状態にする
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// ImGuiの描画開始
	mImGui.begin();

	// 画面をクリア
	auto handle = mSwapChain.getHandle();
	FLOAT clearColors[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	mCmdContext.clearRenderTarget(handle.cpuHandle, clearColors);

	return mCmdContext;
}

void Renderer::end()
{
	auto& context = GraphicsCore::getDeviceContext();

	auto& frameData = mFrameDatas[mCurrentFrameIndex];

	// ImGuiの描画終了
	mImGui.end(mCmdContext.getCommandLists());

	// バリアを元の状態に戻す
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドの書き込みを終了
	mCmdContext.end();

	// ため込んだコマンドを実行していく
	ID3D12CommandList* cmdLists[] = { mCmdContext.getCommandLists() };
	context.getGraphicsQueue()->ExecuteCommandLists(1, cmdLists);

	// 現在の実行状況を値で取得 (同期をとるために必要)
	frameData.fenceValue = context.signalFence();

	// 画面をフリップ
	mSwapChain.present();

	// フレームを次に移動
	mCurrentFrameIndex = (mCurrentFrameIndex + 1) % MaxFrameCount;
}

void Renderer::resizeSwapchain(UINT width, UINT height)
{
	mCurrentWidth = width;
	mCurrentHeight = height;
	mSwapChain.resize(width, height);
}
