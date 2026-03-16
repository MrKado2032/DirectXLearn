#include "pch.h"
#include "GraphicsCore.h"

#include "Util/Helper.h"

namespace GraphicsCore {
	static DeviceContext mContext;
	static DescriptorAllocator mSrvCbvUavAllocator;
	static ResourceUploader mResourceUploader;

	void initialize()
	{
		try {
			mContext.create();
			mResourceUploader.initialize();
			mSrvCbvUavAllocator.create(mContext.getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1000, true);
		}
		catch (std::exception& e) {
			util::Print(e.what());
		}
	}

	void destroy()
	{
		mResourceUploader.destroy();
		mSrvCbvUavAllocator.destroy();
		mContext.destroy();
	}

	DeviceContext& getDeviceContext(){
		return mContext;
	}
	DescriptorAllocator& getSrvCbvUavAllocator(){
		return mSrvCbvUavAllocator;
	}
	ResourceUploader& getResourceUploader(){
		return mResourceUploader;
	}
}