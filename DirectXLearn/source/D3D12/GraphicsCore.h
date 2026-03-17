#pragma once
#include "DeviceContext.h"
#include "DescriptorAllocator.h"
#include "ResourceUploader.h"
#include "D3D12/RootSignatureManager.h"
#include "D3D12/PSOManager.h"

namespace GraphicsCore {

	void initialize();
	void destroy();
	DeviceContext& getDeviceContext();
	DescriptorAllocator& getSrvCbvUavAllocator();
	ResourceUploader& getResourceUploader();
	RootSignatureManager& getRootSignatureManager();
	PSOManager& getPSOManager();
}