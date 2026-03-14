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

	void setRootSignature(ID3D12RootSignature* pRootsignature);
	void transitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	void setViewPort(D3D12_VIEWPORT& viewport);
	void setScissorRect(D3D12_RECT& scissorRect);
	void clearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& handle, FLOAT color[4]);
	void drawInstanced(UINT vertexCount, UINT instanceCount, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
	void drawIndxed(UINT indexCount, UINT instanceCount, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView, D3D12_INDEX_BUFFER_VIEW& indexBufferView);

	ID3D12GraphicsCommandList* getCommandLists() {
		return mCommandList.Get();
	}

private:
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

};