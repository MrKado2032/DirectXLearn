#pragma once

using Microsoft::WRL::ComPtr;

struct Mesh {

	struct VertexData {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	UINT indexCount = 0;

	static D3D12_INPUT_LAYOUT_DESC getInputLayout();

};

class MeshGenerator {
public:
	MeshGenerator() = default;
	~MeshGenerator() = default;

	MeshGenerator(const MeshGenerator&) = delete;
	MeshGenerator operator=(const MeshGenerator&) = delete;

	static std::shared_ptr<Mesh> generateMesh(const std::vector<Mesh::VertexData>& vertices, const std::vector<DWORD>& indices);

private:

};