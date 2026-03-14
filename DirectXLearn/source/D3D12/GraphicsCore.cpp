#include "pch.h"
#include "GraphicsCore.h"

#include "Util/Helper.h"

namespace GraphicsCore {
	static DeviceContext mContext;

	void initialize()
	{
		try {
			mContext.create();
		}
		catch (std::exception& e) {
			util::Print(e.what());
		}
	}

	void destroy()
	{
		mContext.destroy();
	}

	DeviceContext& getDeviceContext(){
		return mContext;
	}
}