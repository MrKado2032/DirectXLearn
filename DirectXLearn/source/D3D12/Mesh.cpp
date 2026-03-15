#include "pch.h"
#include "Mesh.h"

D3D12_INPUT_LAYOUT_DESC Mesh::getInputLayout()
{
	static const D3D12_INPUT_ELEMENT_DESC inputLayouts[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexData, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	return { inputLayouts , _countof(inputLayouts) };
}

Mesh MeshGenerator::generateMesh(const std::vector<Mesh::VertexData>& vertices, const std::vector<DWORD>& indices)
{
	Mesh mesh{};
	mesh.vertices = vertices;
	mesh.indices = indices;

	return mesh;
}
