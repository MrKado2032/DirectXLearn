#include "pch.h"
#include "GraphicsCore.h"

#include "Util/Helper.h"

namespace GraphicsCore {
	static DeviceContext mContext;
	static DescriptorAllocator mTextureDescriptorAllocator;
	static ResourceUploader mResourceUploader;

	void initialize()
	{
		try {
			mContext.create();
			mResourceUploader.initialize();
			mTextureDescriptorAllocator.create(mContext.getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
		}
		catch (std::exception& e) {
			util::Print(e.what());
		}
	}

	void destroy()
	{
		mResourceUploader.destroy();
		mTextureDescriptorAllocator.destroy();
		mContext.destroy();
	}

	DeviceContext& getDeviceContext(){
		return mContext;
	}
	DescriptorAllocator& getTextureDescriptorAllocator(){
		return mTextureDescriptorAllocator;
	}
	ResourceUploader& getResourceUploader(){
		return mResourceUploader;
	}
}