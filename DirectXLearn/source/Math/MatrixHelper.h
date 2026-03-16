#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace MatrixHelper {

	inline XMMATRIX getDefaultViewMatrix() {

		auto eye = DirectX::XMFLOAT3(0, 0, -5);
		auto target = DirectX::XMFLOAT3(0, 0, 0);
		auto up = DirectX::XMFLOAT3(0, 1, 0);
		auto view = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&up));
	
		return view;
	}

	inline XMMATRIX getDefaultProjectionMatrix(int width, int height) {

		auto projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, static_cast<float>(width) / static_cast<float>(height), 1.0f, 1000.0f);

		return projection;
	}

}