#pragma once

using Microsoft::WRL::ComPtr;

class DeviceContext {
public:
	static constexpr uint32_t MaxFrameCount = 2;

	DeviceContext();
	~DeviceContext();

	DeviceContext(const DeviceContext&) = delete;
	DeviceContext operator=(const DeviceContext&) = delete;

	void create();
	void destroy();

	UINT64 flushGPU();
	void waitForFence(UINT64 fenceValue);
	UINT64 signalFence();

	ID3D12Device* getDevice() const { return mDevice.Get(); }
	ID3D12CommandQueue* getGraphicsQueue() const { return mGraphicsQueue.Get(); }
	IDXGIFactory7* getFactory() const { return mFactory.Get(); }

private:
	void setupDebugLayer(UINT& dxgiFlags);
	void createDevice();
	void createGraphicsQueue();
	void createSyncObject();

private:
	ComPtr<ID3D12Device> mDevice;
	ComPtr<IDXGIFactory7> mFactory;
	ComPtr<ID3D12CommandQueue> mGraphicsQueue;

	// 同期オブジェクト
	ComPtr<ID3D12Fence> mFence;
	UINT64 mNextFenceValue = 1;
	HANDLE mFenceEvent = nullptr;
};