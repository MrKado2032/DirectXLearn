#pragma once

using Microsoft::WRL::ComPtr;

struct DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
};

class DescriptorAllocator {
public:
	DescriptorAllocator();
	~DescriptorAllocator();

	DescriptorAllocator(const DescriptorAllocator&) = delete;
	DescriptorAllocator operator=(const DescriptorAllocator&) = delete;

	void create(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool bVisibleShader);
	void destroy();

	DescriptorHandle allocate();
	void free(DescriptorHandle& handle);

	ID3D12DescriptorHeap* getDescriptorHeap() const { return mDescriptorHeap.Get(); }

private:
	ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE mStartCpuHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE mStartGpuHandle{};
	UINT mDescriptorSize = 0;
	uint32_t mMaxDescriptors = 0;

	std::vector<uint32_t> mFreeList;
};