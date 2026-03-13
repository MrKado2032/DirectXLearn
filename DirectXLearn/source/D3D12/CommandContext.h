#pragma once

using Microsoft::WRL::ComPtr;

class CommandContext {
public:
	CommandContext();
	~CommandContext();

	CommandContext(const CommandContext&) = delete;
	CommandContext operator=(const CommandContext&) = delete;

	void create(ID3D12Device* pDevice, ID3D12CommandAllocator* pAllocator, D3D12_COMMAND_LIST_TYPE type);
	void destroy();

	void begin(ID3D12CommandAllocator* pAllocator, ID3D12PipelineState* pPipelineState = nullptr);
	void end();

	void transitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	void clearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& handle, FLOAT color[4]);

	ID3D12GraphicsCommandList* getCommandLists() {
		return mCommandList.Get();
	}

private:
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

};