#include "pch.h"
#include "ResourceUploader.h"

#include "GraphicsCore.h"
#include "Util/Helper.h"

void ResourceUploader::initialize()
{
	auto& context = GraphicsCore::getDeviceContext();

	util::ThrowIfFailed(context.getDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())));
	mCommandContext.create(context.getDevice(), mCommandAllocator.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.NodeMask = 0;

	util::ThrowIfFailed(context.getDevice()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf())));

	util::ThrowIfFailed(context.getDevice()->CreateFence(mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())));
	mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void ResourceUploader::destroy()
{
	if (mFenceEvent) {
		CloseHandle(mFenceEvent);
	}
}

void ResourceUploader::begin()
{
	util::ThrowIfFailed(mCommandAllocator->Reset());
	mCommandContext.begin(mCommandAllocator.Get());
}

void ResourceUploader::finish()
{
	mCommandContext.end();

	ID3D12CommandList* cmdLists[] = { mCommandContext.getCommandLists()};
	mCommandQueue->ExecuteCommandLists(1, cmdLists);

	mCommandQueue->Signal(mFence.Get(), ++mFenceValue);
	if (mFence->GetCompletedValue() < mFenceValue) {
		mFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
		WaitForSingleObject(mFenceEvent, INFINITE);
	}

	mUploadedResources.clear();
}

void ResourceUploader::uploadTextureBuffer(ID3D12Resource* dstResource, const D3D12_SUBRESOURCE_DATA& subresourceData, const UINT64& uploadBufferSize)
{
	auto& context = GraphicsCore::getDeviceContext();

	auto stagingHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto stagingResDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ComPtr<ID3D12Resource> stagingBuffer;
	util::ThrowIfFailed(context.getDevice()->CreateCommittedResource(&stagingHeapProps, D3D12_HEAP_FLAG_NONE, &stagingResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(stagingBuffer.GetAddressOf())));

	UpdateSubresources(mCommandContext.getCommandLists(), dstResource, stagingBuffer.Get(), 0, 0, 1, &subresourceData);

	mCommandContext.transitionBarrier(dstResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	mUploadedResources.push_back(stagingBuffer);
}
