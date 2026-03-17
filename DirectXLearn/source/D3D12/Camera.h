#pragma once

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class Camera {
public:
	Camera(float width, float height);
	~Camera();

	void update();

	void setPos(float x, float y, float z) {
		mPosition = { x, y , z };
	}

	void setTarget(float x, float y, float z) {
		mTarget = { x, y, z };
	}

	void movePos(float x, float y, float z) {
		auto prevPos = mPosition;
		mPosition = { prevPos.x + x, prevPos.y + y, prevPos.z + z };
	}

	void moveForward(float dist) {
		// 注視点(Target) - 現在地(Eye) で「前」を計算
		XMVECTOR eye = XMLoadFloat3(&mPosition);
		XMVECTOR target = XMLoadFloat3(&mTarget);
		XMVECTOR forward = XMVector3Normalize(target - eye);

		// 現在地に「前 × 距離」を足す
		eye += forward * dist;

		// Targetも一緒に動かさないと、見ている場所が固定されてしまう
		target += forward * dist;

		XMStoreFloat3(&mPosition, eye);
		XMStoreFloat3(&mTarget, target);
	}

	void moveRight(float dist) {
		XMVECTOR eye = XMLoadFloat3(&mPosition);
		XMVECTOR target = XMLoadFloat3(&mTarget);
		XMVECTOR up = XMLoadFloat3(&mUp);

		XMVECTOR forward = XMVector3Normalize(target - eye);
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

		eye += right * dist;
		target += right * dist;

		XMStoreFloat3(&mPosition, eye);
		XMStoreFloat3(&mTarget, target);
	}

	void rotate(float dPitch, float dYaw) {
		mPitch += dPitch;
		mYaw += dYaw;

		const float limit = XMConvertToRadians(89.0f);
		if (mPitch > limit) mPitch = limit;
		if (mYaw > limit) mYaw = limit;
	}

	XMMATRIX getViewProjectionMatrix() const {
		return mViewMatrix * mProjectionMatrix;
	}

private:
	XMMATRIX mViewMatrix;
	XMMATRIX mProjectionMatrix;

	XMFLOAT3 mPosition;
	XMFLOAT3 mTarget;
	XMFLOAT3 mUp;
	float mFOV = 0;
	float mAspect = 0;

	float mPitch = 0;
	float mYaw = 0;
};