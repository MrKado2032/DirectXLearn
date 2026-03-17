#include "pch.h"
#include "RootSignatureManager.h"

#include "Util/Helper.h"

void RootSignatureManager::initialize(ID3D12Device* pDevice)
{
	CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[1]{};
	descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	CD3DX12_ROOT_PARAMETER1 rootParams[2]{};
	rootParams[0].InitAsDescriptorTable(_countof(descriptorRanges), descriptorRanges, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParams[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_STATIC_SAMPLER_DESC samplers[1]{};
	samplers->Init(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	rootSigDesc.Init_1_1(_countof(rootParams), rootParams, _countof(samplers), samplers, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBinary, errorBinary;
	util::ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootSigDesc, rootSigBinary.GetAddressOf(), errorBinary.GetAddressOf()));

	util::ThrowIfFailed(pDevice->CreateRootSignature(0, rootSigBinary->GetBufferPointer(), rootSigBinary->GetBufferSize(), IID_PPV_ARGS(mBaseRootSignature.GetAddressOf())));
}

void RootSignatureManager::destroy()
{
	if (mBaseRootSignature) {
		mBaseRootSignature.Reset();
	}
}
