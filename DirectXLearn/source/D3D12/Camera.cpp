#include "pch.h"
#include "Camera.h"

Camera::Camera(float width, float height)
{
	mPosition = { 0.0f, 0.0f, -10.0f };
	mTarget = { 0.0f, 0.0f, 0.0f };
	mUp = { 0.0f, 1.0f, 0.0f };
	mFOV = XMConvertToRadians(45.0f);
	mAspect = width / height;

	update();
}

Camera::~Camera()
{
}

void Camera::update()
{
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(mPitch, mYaw, 0.0f);

	XMVECTOR defaultForward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR forward = XMVector3TransformNormal(defaultForward, rotationMatrix);

	XMVECTOR eye = XMLoadFloat3(&mPosition);
	XMVECTOR target = eye + forward;
	XMStoreFloat3(&mTarget, target);

	XMVECTOR up = XMLoadFloat3(&mUp);
	mViewMatrix = XMMatrixLookAtLH(eye, target, up);

	mProjectionMatrix = XMMatrixPerspectiveFovLH(mFOV, mAspect, 1.0f, 1000.0f);
}
