#pragma once

using Microsoft::WRL::ComPtr;

class RootSignatureManager {
public:
	RootSignatureManager() = default;
	~RootSignatureManager() = default;

	RootSignatureManager(const RootSignatureManager&) = delete;
	RootSignatureManager operator=(const RootSignatureManager&) = delete;

	void initialize(ID3D12Device* pDevice);
	void destroy();

	ID3D12RootSignature* getBaseRootSignature() const { return mBaseRootSignature.Get(); }

private:
	ComPtr<ID3D12RootSignature> mBaseRootSignature;
};