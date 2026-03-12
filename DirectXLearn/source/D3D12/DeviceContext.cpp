#include "pch.h"
#include "DeviceContext.h"

#include "Util/Helper.h"

DeviceContext::DeviceContext()
{
}

DeviceContext::~DeviceContext()
{
}

void DeviceContext::create()
{
	UINT dxgiFlags = 0;			// DXGIデバッグを有効にするための変数
	setupDebugLayer(dxgiFlags);	// ここでデバッグを有効

	// これはグラフィックボードの選択やスワップチェインの作成に使用する
	util::ThrowIfFailed(CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(mFactory.GetAddressOf())));

	// デバイスの作成
	createDevice();

	// グラフィックスキューの作成
	createGraphicsQueue();
}

void DeviceContext::destroy()
{
	// Comptrは自動的に破棄してくれるみたいだが、念のため手動で
	mGraphicsQueue.Reset();
	mDevice.Reset();
	mFactory.Reset();
}

/// <summary>
/// DirectX12のデバック関連を行ってくれる素晴らしいツールのセットアップ
/// </summary>
/// <param name="dxgiFlags"></param>
void DeviceContext::setupDebugLayer(UINT& dxgiFlags)
{
#ifdef _DEBUG
	ComPtr<ID3D12Debug6> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
		debugController->EnableDebugLayer();
		dxgiFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
}

/// <summary>
/// デバイスの作成
/// </summary>
void DeviceContext::createDevice()
{
	// グラフィックボードを探す必要がある
	ComPtr<IDXGIAdapter4> targetAdapter;
	for (UINT i = 0; mFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(targetAdapter.ReleaseAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND; i++) {
		
		DXGI_ADAPTER_DESC3 adpDesc{};		// この中にグラフィックボード情報が入ってる（現段階では空）
		targetAdapter->GetDesc3(&adpDesc);	// 対象のグラボから情報を「adpDesc」に書き込む
		
		// Warpデバイス (外刺しグラボやCPUに入ってるGPUではないソフトウェアで動くもの) はスキップ
		if (adpDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) continue;

		// 仮デバイスを作って成功したらそのグラボ選ぶ
		if (SUCCEEDED(D3D12CreateDevice(targetAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) {

			util::Print(L"Find Adapter: {}", adpDesc.Description);	// 選択したグラボ名を出力してみる。

			break;
		}
	}

	// 実際にデバイスを作成
	util::ThrowIfFailed(D3D12CreateDevice(targetAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.GetAddressOf())));
}

/// <summary>
/// グラフィックスキューの作成
/// </summary>
void DeviceContext::createGraphicsQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;			// いわゆる汎用型キューってやつ？ほかにも「コンピュート用」とか「コピー用」があるみたい。
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;			// 現段階では意味が分からん
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// キューの優先度っぽいけど、現段階では意味が分からん
	queueDesc.NodeMask = 0;										// マルチGPUを有効にしたいときに使うみたい。私は1個しか使わないから「0」でOKみたい。

	util::ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(mGraphicsQueue.GetAddressOf())));
}
