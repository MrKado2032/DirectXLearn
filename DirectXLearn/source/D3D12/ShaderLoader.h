#pragma once
#include <dxcapi.h>

using Microsoft::WRL::ComPtr;

struct ShaderData {
	std::vector<char> vertexShader;
	std::vector<char> pixelShader;
};

class ShaderLoader {
public:
	ShaderLoader() = default;
	~ShaderLoader() = default;

	ShaderLoader(const ShaderLoader&) = delete;
	ShaderLoader operator=(const ShaderLoader&) = delete;
	
	ShaderData loadShaderFromFile(const std::wstring& filePath);

private:
	std::vector<char> compileWithDxc(const std::wstring& filePath, const std::string& entryPoint, const std::string& target);

	ComPtr<IDxcUtils> mDxcUtils;
	ComPtr<IDxcCompiler3> mDxcCompiler;
};