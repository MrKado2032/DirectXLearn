#include "pch.h"
#include "Model.h"

#include "GraphicsCore.h"
#include "Buffer.h"

Model ModelGenerator::generateModel(const Mesh& mesh, const Material& material)
{
	Model model{};
	model.material = material;

	auto const& context = GraphicsCore::getDeviceContext();

	size_t vertexBufferSize = mesh.vertices.size() * sizeof(Mesh::VertexData);

	util::BufferCreateInfo vertexBufferCI{};
	vertexBufferCI.width = static_cast<UINT64>(vertexBufferSize);
	vertexBufferCI.height = 1;
	vertexBufferCI.heapType = D3D12_HEAP_TYPE_UPLOAD;
	vertexBufferCI.dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexBufferCI.format = DXGI_FORMAT_UNKNOWN;
	vertexBufferCI.layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexBufferCI.initState = D3D12_RESOURCE_STATE_GENERIC_READ;
	vertexBufferCI.bMapping = TRUE;

	util::createBuffer(context.getDevice(), vertexBufferCI, mesh.vertices.data(), model.vertexBuffer.GetAddressOf());

	model.vbView.BufferLocation = model.vertexBuffer->GetGPUVirtualAddress();
	model.vbView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
	model.vbView.StrideInBytes = static_cast<UINT>(sizeof(Mesh::VertexData));

	size_t indexBufferSize = mesh.indices.size() * sizeof(DWORD);

	util::BufferCreateInfo indexBufferCI{};
	indexBufferCI.width = static_cast<UINT64>(indexBufferSize);
	indexBufferCI.height = 1;
	indexBufferCI.heapType = D3D12_HEAP_TYPE_UPLOAD;
	indexBufferCI.dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	indexBufferCI.format = DXGI_FORMAT_UNKNOWN;
	indexBufferCI.layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	indexBufferCI.initState = D3D12_RESOURCE_STATE_GENERIC_READ;
	indexBufferCI.bMapping = TRUE;

	util::createBuffer(context.getDevice(), indexBufferCI, mesh.indices.data(), model.indexBuffer.GetAddressOf());

	model.ibView.BufferLocation = model.indexBuffer->GetGPUVirtualAddress();
	model.ibView.SizeInBytes = static_cast<UINT>(indexBufferSize);
	model.ibView.Format = DXGI_FORMAT_R32_UINT;

	model.indexCount = static_cast<UINT>(mesh.indices.size());

	return model;
}
