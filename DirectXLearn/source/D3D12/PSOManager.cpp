#include "pch.h"
#include "PSOManager.h"

#include "D3D12/Mesh.h"
#include "Util/Helper.h"

void PSOManager::initialize(ID3D12Device* pDevice, ID3D12RootSignature* pBaseRootSignature)
{
	// シェーダーのコンパイル
	auto shaderData = shaderLoader.loadShaderFromFile(L"source/Shaders/BasicShader.hlsl");

	// パイプラインステートの作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = pBaseRootSignature;
	psoDesc.NodeMask = 0;
	psoDesc.InputLayout = Mesh::getInputLayout();
	psoDesc.VS = { shaderData.vertexShader.data(), shaderData.vertexShader.size() };
	psoDesc.PS = { shaderData.pixelShader.data(), shaderData.pixelShader.size() };
	psoDesc.NumRenderTargets = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	createMultiPipeline(pDevice, psoDesc);
}

void PSOManager::destroy()
{
	for (auto& pso : mPsoList) {
		pso.second.Reset();
	}
	mPsoList.clear();
}

ID3D12PipelineState* PSOManager::findPSOAndGet(const std::string& key) const
{
	auto it = mPsoList.find(key);
	if (it != mPsoList.end()) {
		return it->second.Get();
	}

	return nullptr;
}

void PSOManager::createMultiPipeline(ID3D12Device* pDevice, D3D12_GRAPHICS_PIPELINE_STATE_DESC baseDesc)
{
	// 2D用Pipeline
	auto pso2d = PSOBuilder::buildPSO(pDevice, baseDesc);
	mPsoList.emplace(PIPELINE_KEY_2DBASE, pso2d);

	// 3D用Pipeline
	baseDesc.NodeMask = 0;
	baseDesc.InputLayout = Mesh::getInputLayout();

	auto pso3d = PSOBuilder::buildPSO(pDevice, baseDesc);
	mPsoList.emplace(PIPELINE_KEY_3DBASE, pso3d);
}

ComPtr<ID3D12PipelineState> PSOBuilder::buildPSO(ID3D12Device* pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
{
	ComPtr<ID3D12PipelineState> pso;
	util::ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pso.GetAddressOf())));

	return pso;
}
