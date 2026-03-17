#include "pch.h"
#include "GraphicsCore.h"

#include "Util/Helper.h"

namespace GraphicsCore {
	static DeviceContext mContext;
	static DescriptorAllocator mSrvCbvUavAllocator;
	static ResourceUploader mResourceUploader;
	static RootSignatureManager mRootSignatureManager;
	static PSOManager mPSOManager;

	void initialize()
	{
		try {
			mContext.create();
			mResourceUploader.initialize();
			mSrvCbvUavAllocator.create(mContext.getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1000, true);

			// ルートシグネチャの作成
			mRootSignatureManager.initialize(mContext.getDevice());

			// パイプラインステートの作成
			mPSOManager.initialize(mContext.getDevice(), mRootSignatureManager.getBaseRootSignature());
		}
		catch (std::exception& e) {
			util::Print(e.what());
		}
	}

	void destroy()
	{
		mPSOManager.destroy();
		mRootSignatureManager.destroy();
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

	RootSignatureManager& getRootSignatureManager(){
		return mRootSignatureManager;
	}

	PSOManager& getPSOManager(){
		return mPSOManager;
	}
}