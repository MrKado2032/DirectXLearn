#include "pch.h"
#include "Mesh.h"

#include "GraphicsCore.h"
#include "Buffer.h"

D3D12_INPUT_LAYOUT_DESC Mesh::getInputLayout()
{
	static const D3D12_INPUT_ELEMENT_DESC inputLayouts[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexData, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	return { inputLayouts , _countof(inputLayouts) };
}

std::shared_ptr<Mesh> MeshGenerator::generateMesh(const std::vector<Mesh::VertexData>& vertices, const std::vector<DWORD>& indices)
{
	auto mesh = std::make_shared<Mesh>();

	auto const& context = GraphicsCore::getDeviceContext();

	size_t vertexBufferSize = vertices.size() * sizeof(Mesh::VertexData);

	util::BufferCreateInfo vertexBufferCI{};
	vertexBufferCI.width = static_cast<UINT64>(vertexBufferSize);
	vertexBufferCI.height = 1;
	vertexBufferCI.heapType = D3D12_HEAP_TYPE_UPLOAD;
	vertexBufferCI.dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexBufferCI.format = DXGI_FORMAT_UNKNOWN;
	vertexBufferCI.layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexBufferCI.initState = D3D12_RESOURCE_STATE_GENERIC_READ;
	vertexBufferCI.bMapping = TRUE;

	util::createBuffer(context.getDevice(), vertexBufferCI, vertices.data(), mesh->vertexBuffer.GetAddressOf());

	mesh->vbView.BufferLocation = mesh->vertexBuffer->GetGPUVirtualAddress();
	mesh->vbView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
	mesh->vbView.StrideInBytes = static_cast<UINT>(sizeof(Mesh::VertexData));

	size_t indexBufferSize = indices.size() * sizeof(DWORD);

	util::BufferCreateInfo indexBufferCI{};
	indexBufferCI.width = static_cast<UINT64>(indexBufferSize);
	indexBufferCI.height = 1;
	indexBufferCI.heapType = D3D12_HEAP_TYPE_UPLOAD;
	indexBufferCI.dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	indexBufferCI.format = DXGI_FORMAT_UNKNOWN;
	indexBufferCI.layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	indexBufferCI.initState = D3D12_RESOURCE_STATE_GENERIC_READ;
	indexBufferCI.bMapping = TRUE;

	util::createBuffer(context.getDevice(), indexBufferCI, indices.data(), mesh->indexBuffer.GetAddressOf());

	mesh->ibView.BufferLocation = mesh->indexBuffer->GetGPUVirtualAddress();
	mesh->ibView.SizeInBytes = static_cast<UINT>(indexBufferSize);
	mesh->ibView.Format = DXGI_FORMAT_R32_UINT;

	mesh->indexCount = static_cast<UINT>(indices.size());

	return mesh;
}
