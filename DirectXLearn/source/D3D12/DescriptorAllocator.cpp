#include "pch.h"
#include "DescriptorAllocator.h"

#include "Util/Helper.h"

DescriptorAllocator::DescriptorAllocator()
{
}

DescriptorAllocator::~DescriptorAllocator()
{
}

void DescriptorAllocator::create(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool bVisibleShader)
{
	if (util::nullCheck(pDevice)) {
		util::Print("Failed to create descriptorHeap: device is nullptr.");
		return;
	}

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV || type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) bVisibleShader = false;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = type;																							// ヒープのタイプ
	heapDesc.NumDescriptors = static_cast<UINT>(numDescriptors);													// 確保する個数
	heapDesc.Flags = bVisibleShader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// シェーダーから見れるか
	heapDesc.NodeMask = 0;																							// マルチGPU

	util::ThrowIfFailed(pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf())));

	mDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(type);	// ヒープタイプからディスクリプタサイズを取得 (確保・解放の計算に使用)
	mMaxDescriptors = numDescriptors;

	mStartCpuHandle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();	// CPUのディスクリプタヒープの先頭アドレスを保持
	if (bVisibleShader) {
		mStartGpuHandle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();// GPUのディスクリプタヒープの先頭アドレスを保持
	}

	// 解放リストはすべて空き状態にする
	mFreeList.resize(numDescriptors);
	for (uint32_t i = 0; i < numDescriptors; i++) {
		mFreeList[i] = i;
	}
}

void DescriptorAllocator::destroy()
{
	if (mDescriptorHeap) {
		mDescriptorHeap.Reset();
	}
}

DescriptorHandle DescriptorAllocator::allocate()
{
	DescriptorHandle handle{};
	
	// 解放リストに空きがあるか確認
	if (mFreeList.empty()) {
		util::Print("Descriptor heap is full. No free handles available.");
	}

	uint32_t index = mFreeList.back();	// 解放リストの末尾からインデックス取得
	mFreeList.pop_back();				// 末尾の要素を削除

	handle.cpuHandle.ptr = mStartCpuHandle.ptr + static_cast<SIZE_T>(index * mDescriptorSize);
	if (mStartGpuHandle.ptr != 0) {
		handle.gpuHandle.ptr = mStartGpuHandle.ptr + static_cast<SIZE_T>(index * mDescriptorSize);
	}

	return handle;
}

void DescriptorAllocator::free(DescriptorHandle& handle)
{
	auto index = (handle.cpuHandle.ptr - mStartCpuHandle.ptr) / static_cast<SIZE_T>(mDescriptorSize);	// ハンドルからディスクリプタヒープに格納されているインデックスを計算

	assert(index < mMaxDescriptors);

	mFreeList.push_back(static_cast<uint32_t>(index));
	handle = {};
}
