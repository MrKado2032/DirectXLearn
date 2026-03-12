#pragma once

using Microsoft::WRL::ComPtr;

class DeviceContext {
public:
	DeviceContext();
	~DeviceContext();

	DeviceContext(const DeviceContext&) = delete;
	DeviceContext operator=(const DeviceContext&) = delete;

	void create();
	void destroy();

private:
	void setupDebugLayer(UINT& dxgiFlags);
	void createDevice();
	void createGraphicsQueue();

private:
	ComPtr<ID3D12Device> mDevice;
	ComPtr<IDXGIFactory7> mFactory;
	ComPtr<ID3D12CommandQueue> mGraphicsQueue;
};