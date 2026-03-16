#include "pch.h"
#include "Model.h"

#include "GraphicsCore.h"
#include "Buffer.h"
#include "Math/MatrixHelper.h"

Model ModelGenerator::generateModel(const std::shared_ptr<Mesh>& mesh, const Material& material)
{
	Model model{};
	model.mesh = mesh;
	model.material = material;

	auto const& context = GraphicsCore::getDeviceContext();

	// 定数バッファーの作成
	model.transform.constBuffer = std::make_unique<util::ConstantBuffer<DirectX::XMMATRIX>>();
	model.transform.constBuffer->create(context.getDevice());
	
	auto view = MatrixHelper::getDefaultViewMatrix();
	auto projection = MatrixHelper::getDefaultProjectionMatrix(1280, 720);

	model.transform.worldMatrix = DirectX::XMMatrixScaling(2, 1.3f, 1) * DirectX::XMMatrixRotationY(0.0f) * DirectX::XMMatrixTranslation(0, 0, 0);
	model.transform.update(view * projection);

	return model;
}

void Model::draw(CommandContext& ctx)
{
	ctx.setConstantBuffer(1, transform.constBuffer->getGpuVirtualAddress());
	ctx.setDescriptorTable(0, material.texture.handle.gpuHandle);
	ctx.drawIndxed(mesh->indexCount, 1, mesh->vbView, mesh->ibView);
}
