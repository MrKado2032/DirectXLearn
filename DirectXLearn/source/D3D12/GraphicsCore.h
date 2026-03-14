#pragma once
#include "DeviceContext.h"

namespace GraphicsCore {

	void initialize();
	void destroy();
	DeviceContext& getDeviceContext();
}