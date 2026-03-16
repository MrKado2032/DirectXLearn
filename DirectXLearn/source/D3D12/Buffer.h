#pragma once

#include "Util/Helper.h"

using Microsoft::WRL::ComPtr;

namespace util {


	struct BufferCreateInfo {
		D3D12_HEAP_TYPE heapType;
		UINT64 width;
		UINT height = 1;
		D3D12_RESOURCE_DIMENSION dimension;
		DXGI_FORMAT format;
		D3D12_TEXTURE_LAYOUT layout;
		D3D12_RESOURCE_STATES initState;
		UINT16 mipLevels = 1;
		UINT16 depthArraySize = 1;
		BOOL bMapping = TRUE;
	};

	template<typename T>
	inline void createBuffer(ID3D12Device* pDevice, const BufferCreateInfo& info, const T* datas, ID3D12Resource** ppResource) {
		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = info.heapType;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.VisibleNodeMask = 1;
		heapProps.CreationNodeMask = 1;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Width = info.width;
		resDesc.Height = info.height;
		resDesc.Dimension = info.dimension;
		resDesc.Format = info.format;
		resDesc.MipLevels = info.mipLevels;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Alignment = 0;
		resDesc.DepthOrArraySize = info.depthArraySize;
		resDesc.Layout = info.layout;

		util::ThrowIfFailed(pDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, info.initState, nullptr, IID_PPV_ARGS(ppResource)));

		if (!info.bMapping) {
			return;
		}

		T* mappedData = nullptr;
		util::ThrowIfFailed((*ppResource)->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
		std::memcpy(mappedData, datas, info.width);
		(*ppResource)->Unmap(0, nullptr);
	}

	template<typename T>
	class ConstantBuffer {
	public:
		ConstantBuffer() {

		}
		~ConstantBuffer() {

		}

		ConstantBuffer(const ConstantBuffer&) = delete;
		ConstantBuffer operator=(const ConstantBuffer&) = delete;

		void create(ID3D12Device* pDevice) {

			const UINT64 bufferSize = (sizeof(T) + 0xff) & ~0xff;

			auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			util::ThrowIfFailed(
				pDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(constBuffer.GetAddressOf()))
			);

			if (mappedData != nullptr) {
				mappedData = nullptr;
			}

			util::ThrowIfFailed(constBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
		}

		void update(const T& data) {
			if (mappedData) {
				std::memcpy(mappedData, &data, sizeof(T));
			}
		}

		void destroy() {
			if (constBuffer) {
				constBuffer->Unmap(0, nullptr);
				mappedData = nullptr;
				constBuffer.Reset();
			}
		}

		D3D12_GPU_VIRTUAL_ADDRESS getGpuVirtualAddress() {
			return constBuffer->GetGPUVirtualAddress();
		}

		void createView(ID3D12Device* pDevice, D3D12_CPU_DESCRIPTOR_HANDLE handle) {
			D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc{};
			viewDesc.BufferLocation = constBuffer->GetGPUVirtualAddress();
			viewDesc.SizeInBytes = (sizeof(T) + 0xff) & ~0xff;
			pDevice->CreateConstantBufferView(&viewDesc, handle);
		}

	private:
		T* mappedData = nullptr;
		ComPtr<ID3D12Resource> constBuffer;
	};

}