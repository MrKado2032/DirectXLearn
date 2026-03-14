#pragma once

struct Mesh {

	struct VertexData {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 col;
	};

	std::vector<VertexData> vertices;
	std::vector<DWORD> indices;

	static D3D12_INPUT_LAYOUT_DESC getInputLayout();

};

class MeshGenerator {
public:
	MeshGenerator() = default;
	~MeshGenerator() = default;

	MeshGenerator(const MeshGenerator&) = delete;
	MeshGenerator operator=(const MeshGenerator&) = delete;

	static Mesh generateMesh(const std::vector<Mesh::VertexData>& vertices, const std::vector<DWORD>& indices);

private:

};