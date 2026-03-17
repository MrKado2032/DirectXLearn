#pragma once
#include "D3D12/ShaderLoader.h"

using Microsoft::WRL::ComPtr;

class PSOManager {
public:
	PSOManager() = default;
	~PSOManager() = default;

	PSOManager(const PSOManager&) = delete;
	PSOManager operator=(const PSOManager&) = delete;

	void initialize(ID3D12Device* pDevice, ID3D12RootSignature* pBaseRootSignature);
	void destroy();

	ID3D12PipelineState* findPSOAndGet(const std::string& key) const;

private:
	void createMultiPipeline(ID3D12Device* pDevice, D3D12_GRAPHICS_PIPELINE_STATE_DESC baseDesc);

private:
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPsoList;
	ShaderLoader shaderLoader;
};

class PSOBuilder {
public:
	PSOBuilder() = default;
	~PSOBuilder() = default;

	PSOBuilder(const PSOBuilder&) = delete;
	PSOBuilder operator=(const PSOBuilder&) = delete;

	static ComPtr<ID3D12PipelineState> buildPSO(ID3D12Device* pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
};

#define PIPELINE_KEY_2DBASE "2DBasePipeline"
#define PIPELINE_KEY_3DBASE "3DBasePipeline"