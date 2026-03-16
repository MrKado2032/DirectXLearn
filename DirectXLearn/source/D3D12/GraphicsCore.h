#pragma once
#include "DeviceContext.h"
#include "DescriptorAllocator.h"
#include "ResourceUploader.h"

namespace GraphicsCore {

	void initialize();
	void destroy();
	DeviceContext& getDeviceContext();
	DescriptorAllocator& getSrvCbvUavAllocator();
	ResourceUploader& getResourceUploader();
}