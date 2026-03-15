#pragma once
#include "DescriptorAllocator.h"

using Microsoft::WRL::ComPtr;

struct Texture {
	ComPtr<ID3D12Resource> resource;
	DescriptorHandle handle{};
};

class TextureLoader {
public:
	TextureLoader() = default;
	~TextureLoader() = default;

	TextureLoader(const TextureLoader&) = delete;
	TextureLoader operator=(const TextureLoader&) = delete;

	static Texture loadTextureFromFile(const std::wstring& filePath);

private:
	static Texture loadTextureDataFromFile(const std::wstring& filePath);
	static void createTexture(Texture& texture);
};