#include "pch.h"
#include "Texture.h"

#include "GraphicsCore.h"
#include "Util/Helper.h"
#include "Buffer.h"
#include <DirectXTex.h>

Texture TextureLoader::loadTextureFromFile(const std::wstring& filePath)
{
	auto texture = loadTextureDataFromFile(filePath);
	createTexture(texture);

	return texture;
}

Texture TextureLoader::loadTextureDataFromFile(const std::wstring& filePath)
{
	// WICテクスチャのロード
	DirectX::TexMetadata metaData{};
	DirectX::ScratchImage scratchImage{};

	util::ThrowIfFailed(DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, &metaData, scratchImage));

	auto img = scratchImage.GetImage(0, 0, 0);

	auto& context = GraphicsCore::getDeviceContext();
	auto& resourceUploader = GraphicsCore::getResourceUploader();

	util::BufferCreateInfo textureBufferCI{};
	textureBufferCI.heapType = D3D12_HEAP_TYPE_DEFAULT;
	textureBufferCI.width = static_cast<UINT64>(metaData.width);
	textureBufferCI.height = static_cast<UINT64>(metaData.height);
	textureBufferCI.dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	textureBufferCI.format = metaData.format;
	textureBufferCI.depthArraySize = metaData.arraySize;
	textureBufferCI.initState = D3D12_RESOURCE_STATE_COPY_DEST;
	textureBufferCI.layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureBufferCI.bMapping = FALSE;

	Texture texture{};
	util::createBuffer(context.getDevice(), textureBufferCI, img, texture.resource.GetAddressOf());

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.resource.Get(), 0, 1);

	D3D12_SUBRESOURCE_DATA textureData{};
	textureData.pData = img->pixels;
	textureData.RowPitch = img->rowPitch;
	textureData.SlicePitch = img->slicePitch;

	resourceUploader.begin();
	resourceUploader.uploadTextureBuffer(texture.resource.Get(), textureData, uploadBufferSize);
	resourceUploader.finish();

	return texture;
}

void TextureLoader::createTexture(Texture& texture)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture.resource->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texture.resource->GetDesc().MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	auto& textureAllocator = GraphicsCore::getSrvCbvUavAllocator();
	auto& context = GraphicsCore::getDeviceContext();

	auto descriptorHandle = textureAllocator.allocate();
	context.getDevice()->CreateShaderResourceView(texture.resource.Get(), &srvDesc, descriptorHandle.cpuHandle);

	texture.handle = descriptorHandle;
}
