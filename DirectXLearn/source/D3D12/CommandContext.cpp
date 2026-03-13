#include "pch.h"
#include "CommandContext.h"

#include "Util/Helper.h"

CommandContext::CommandContext()
{
}

CommandContext::~CommandContext()
{
}

void CommandContext::create(ID3D12Device* pDevice, ID3D12CommandAllocator* pAllocator, D3D12_COMMAND_LIST_TYPE type)
{
	// コマンドリストの作成
	util::ThrowIfFailed(pDevice->CreateCommandList(0, type, pAllocator, nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));
	util::ThrowIfFailed(mCommandList->Close());
}

void CommandContext::destroy()
{
	if (mCommandList) {
		mCommandList.Reset();
	}
}

void CommandContext::begin(ID3D12CommandAllocator* pAllocator, ID3D12PipelineState* pPipelineState)
{
	util::ThrowIfFailed(mCommandList->Reset(pAllocator, pPipelineState));
}

void CommandContext::end()
{
	util::ThrowIfFailed(mCommandList->Close());	// これ以上コマンドを書き込めないように閉じる
}

/// <summary>
/// バリアの遷移
/// </summary>
/// <param name="pResource"></param>
/// <param name="before"></param>
/// <param name="after"></param>
void CommandContext::transitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = pResource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	mCommandList->ResourceBarrier(1, &barrier);
}

/// <summary>
/// レンダーターゲットビューのクリア
/// </summary>
/// <param name="handle"></param>
/// <param name="color"></param>
void CommandContext::clearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& handle, FLOAT color[4])
{
	// レンダーターゲットビューのセット
	mCommandList->OMSetRenderTargets(1, &handle, TRUE, nullptr);

	// 画面をクリア
	mCommandList->ClearRenderTargetView(handle, color, 0, nullptr);
}
