#pragma once
#include "Mesh.h"
#include "Material.h"

using Microsoft::WRL::ComPtr;

struct Model {
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	UINT indexCount = 0;
	Material material{};
};


class ModelGenerator {
public:
	ModelGenerator() = default;
	~ModelGenerator() = default;

	ModelGenerator(const ModelGenerator&) = delete;
	ModelGenerator operator=(const ModelGenerator&) = delete;

	static Model generateModel(const Mesh& mesh, const Material& material);

private:

};