#include "pch.h"
#include "ShaderLoader.h"

#include <filesystem>
#include "Util/Helper.h"

ShaderData ShaderLoader::loadShaderFromFile(const std::wstring& filePath)
{
	ShaderData data{};
	data.vertexShader = compileWithDxc(filePath, "VSMain", "vs_6_0");
	data.pixelShader = compileWithDxc(filePath, "PSMain", "ps_6_0");

	return data;
}

std::vector<char> ShaderLoader::compileWithDxc(const std::wstring& filePath, const std::string& entryPoint, const std::string& target)
{
	if (!mDxcUtils || !mDxcCompiler) {
		util::ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(mDxcUtils.GetAddressOf())));
		util::ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(mDxcCompiler.GetAddressOf())));
	}

	ComPtr<IDxcIncludeHandler> includeHandler;
	util::ThrowIfFailed(mDxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf()));

	uint32_t codePage = DXC_CP_UTF8;
	ComPtr<IDxcBlobEncoding> source;
	util::ThrowIfFailed(mDxcUtils->LoadFile(filePath.c_str(), &codePage, source.GetAddressOf()));

	const std::wstring wEntryPonit = std::wstring(entryPoint.begin(), entryPoint.end());
	const std::wstring wTarget = std::wstring(target.begin(), target.end());

	std::vector<LPCWSTR> arguments = {
		filePath.c_str(),			// ソース名
		L"-E", wEntryPonit.c_str(),	// エントリーポイント名
		L"-T", wTarget.c_str(),		// ターゲット名
		L"-Zi",						// デバッグ情報
		L"-Qembed_debug",			// デバッグ情報をバイナリに埋め込む
		L"Od"						// 最適化オフ (デバッグ用)
	};

	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = source->GetBufferPointer();
	sourceBuffer.Size = source->GetBufferSize();
	sourceBuffer.Encoding = 0;

	ComPtr<IDxcResult> result;
	util::ThrowIfFailed(mDxcCompiler->Compile(&sourceBuffer, arguments.data(), static_cast<UINT32>(arguments.size()), includeHandler.Get(), IID_PPV_ARGS(result.GetAddressOf())));

	ComPtr<IDxcBlobUtf8> errors;
	util::ThrowIfFailed(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr));
	if (errors && errors->GetStringLength() > 0) {
		util::Print("DXC Compiler Error: {}", (char*)errors->GetStringPointer());
	}

	HRESULT status = 0;
	util::ThrowIfFailed(result->GetStatus(&status));
	if (FAILED(status)) {
		return {};
	}

	ComPtr<IDxcBlob> blob;
	util::ThrowIfFailed(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr));

	const char* start = static_cast<char*>(blob->GetBufferPointer());
	return std::vector<char>(start, start + blob->GetBufferSize());
}
