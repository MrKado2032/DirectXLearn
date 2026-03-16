#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Buffer.h"

using Microsoft::WRL::ComPtr;

struct Transform {
	std::unique_ptr<util::ConstantBuffer<DirectX::XMMATRIX>> constBuffer;
	DirectX::XMMATRIX worldMatrix;

	void update(const DirectX::XMMATRIX& viewProj) const {
		auto mvp = worldMatrix * viewProj;
		constBuffer->update(DirectX::XMMatrixTranspose(mvp));
	}
};

class CommandContext;
struct Model {
	std::shared_ptr<Mesh> mesh;
	Material material{};
	Transform transform;

	void draw(CommandContext& ctx);
};


class ModelGenerator {
public:
	ModelGenerator() = default;
	~ModelGenerator() = default;

	ModelGenerator(const ModelGenerator&) = delete;
	ModelGenerator operator=(const ModelGenerator&) = delete;

	static Model generateModel(const std::shared_ptr<Mesh>& mesh, const Material& material);

private:

};