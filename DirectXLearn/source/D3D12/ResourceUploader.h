#pragma once
#include "CommandContext.h"

using Microsoft::WRL::ComPtr;

class ResourceUploader {
public:
	ResourceUploader() = default;
	~ResourceUploader() = default;

	ResourceUploader(const ResourceUploader&) = delete;
	ResourceUploader operator=(const ResourceUploader&) = delete;

	void initialize();
	void destroy();

	void begin();
	void finish();

	void uploadTextureBuffer(ID3D12Resource* dstResource, const D3D12_SUBRESOURCE_DATA& subresourceData, const UINT64& uploadBufferSize);

private:
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	CommandContext mCommandContext;
	ComPtr<ID3D12CommandQueue> mCommandQueue;

	ComPtr<ID3D12Fence> mFence;
	UINT64 mFenceValue = 0;
	HANDLE mFenceEvent = nullptr;

	std::vector<ComPtr<ID3D12Resource>> mUploadedResources;
};