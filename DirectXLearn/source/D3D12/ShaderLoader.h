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
	
	static ShaderData loadShaderFromFile(const std::wstring& filePath);

private:
	static std::vector<char> compileWithDxc(const std::wstring& filePath, const std::string& entryPoint, const std::string& target);

	static ComPtr<IDxcUtils> mDxcUtils;
	static ComPtr<IDxcCompiler3> mDxcCompiler;
};