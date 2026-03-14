#include "pch.h"
#include "Renderer.h"

#include "Util/Helper.h"
#include "D3D12/ShaderLoader.h"

struct VertexData {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 col;
};

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::create(uint32_t width, uint32_t height, HWND hWnd)
{;
	try {
		mContext.create();
		mSwapChain.create(mContext, width, height, hWnd);
	
		// コマンドアロケーターの作成
		auto device = mContext.getDevice();
		for (auto& frame : mFrameDatas) {
			util::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(frame.commandAllocator.GetAddressOf())));
		}

		mCmdContext.create(mContext.getDevice(), mFrameDatas[0].commandAllocator.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);


		// ルートシグネチャの作成 (後にリファクタリング) 
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc{};
		rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSigDesc.Desc_1_1.NumParameters = 0;
		rootSigDesc.Desc_1_1.pParameters = nullptr;
		rootSigDesc.Desc_1_1.NumStaticSamplers = 0;
		rootSigDesc.Desc_1_1.pStaticSamplers = nullptr;
		rootSigDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> rootSigBinary, errorBinary;
		util::ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootSigDesc, rootSigBinary.GetAddressOf(), errorBinary.GetAddressOf()));

		util::ThrowIfFailed(mContext.getDevice()->CreateRootSignature(0, rootSigBinary->GetBufferPointer(), rootSigBinary->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));

		D3D12_INPUT_ELEMENT_DESC inputLayouts[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexData, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		// シェーダーのコンパイル
		auto shaderData = ShaderLoader::loadShaderFromFile(L"source/Shaders/BasicShader.hlsl");

		// パイプラインステートの作成 (後にリファクタリング)
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.pRootSignature = mRootSignature.Get();
		psoDesc.NodeMask = 0;
		psoDesc.InputLayout.NumElements = _countof(inputLayouts);
		psoDesc.InputLayout.pInputElementDescs = inputLayouts;
		psoDesc.VS.BytecodeLength = shaderData.vertexShader.size();
		psoDesc.VS.pShaderBytecode = shaderData.vertexShader.data();
		psoDesc.PS.BytecodeLength = shaderData.pixelShader.size();
		psoDesc.PS.pShaderBytecode = shaderData.pixelShader.data();
		psoDesc.NumRenderTargets = 1;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesc.BlendState.IndependentBlendEnable = FALSE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		util::ThrowIfFailed(mContext.getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mPipelineState.GetAddressOf())));

		// 頂点バッファーの作成
		std::vector<VertexData> vertices = {
			{{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // left-top
			{{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // right-top
			{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // right-down
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}}, // left-down
		};

		D3D12_HEAP_PROPERTIES vertexHeapProps{};
		vertexHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		vertexHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		vertexHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		vertexHeapProps.VisibleNodeMask = 1;
		vertexHeapProps.CreationNodeMask = 1;

		D3D12_RESOURCE_DESC vertexResDesc{};
		vertexResDesc.Width = static_cast<UINT64>(vertices.size() * sizeof(VertexData));
		vertexResDesc.Height = 1;
		vertexResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vertexResDesc.Format = DXGI_FORMAT_UNKNOWN;
		vertexResDesc.MipLevels = 1;
		vertexResDesc.SampleDesc.Count = 1;
		vertexResDesc.SampleDesc.Quality = 0;
		vertexResDesc.Alignment = 0;
		vertexResDesc.DepthOrArraySize = 1;
		vertexResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		util::ThrowIfFailed(mContext.getDevice()->CreateCommittedResource(&vertexHeapProps, D3D12_HEAP_FLAG_NONE, &vertexResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertexBuffer.GetAddressOf())));

		VertexData* vertMapped = nullptr;
		util::ThrowIfFailed(vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertMapped)));
		std::memcpy(vertMapped, vertices.data(), vertices.size() * sizeof(VertexData));
		vertexBuffer->Unmap(0, nullptr);

		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(VertexData));
		vbView.StrideInBytes = static_cast<UINT>(sizeof(VertexData));

		// インデックスバッファーの作成
		std::vector<DWORD> indices = {
			0, 2, 3, 0, 1, 2
		};

		D3D12_HEAP_PROPERTIES indexHeapProps{};
		indexHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		indexHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		indexHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		indexHeapProps.VisibleNodeMask = 1;
		indexHeapProps.CreationNodeMask = 1;

		D3D12_RESOURCE_DESC indexResDesc{};
		indexResDesc.Width = static_cast<UINT64>(indices.size() * sizeof(DWORD));
		indexResDesc.Height = 1;
		indexResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		indexResDesc.Format = DXGI_FORMAT_UNKNOWN;
		indexResDesc.MipLevels = 1;
		indexResDesc.SampleDesc.Count = 1;
		indexResDesc.SampleDesc.Quality = 0;
		indexResDesc.Alignment = 0;
		indexResDesc.DepthOrArraySize = 1;
		indexResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		util::ThrowIfFailed(mContext.getDevice()->CreateCommittedResource(&indexHeapProps, D3D12_HEAP_FLAG_NONE, &indexResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(indexBuffer.GetAddressOf())));

		DWORD* indexMapped = nullptr;
		util::ThrowIfFailed(indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indexMapped)));
		std::memcpy(indexMapped, indices.data(), indices.size() * sizeof(DWORD));
		indexBuffer->Unmap(0, nullptr);

		ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		ibView.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(DWORD));
		ibView.Format = DXGI_FORMAT_R32_UINT;
	}
	catch (std::exception& e) {
		util::Print(e.what());
	}
}

void Renderer::destroy()
{
	// 今ある実行中の処理をすべて待つ
	UINT64 completedValue = mContext.flushGPU();
	for (auto& frame : mFrameDatas) {
		frame.fenceValue = completedValue;
	}

	mSwapChain.destroy();
	mCmdContext.destroy();
	mContext.destroy();
}

void Renderer::begin()
{
	auto& frameData = mFrameDatas[mCurrentFrameIndex];	// 現在のフレームデータを取得
	mContext.waitForFence(frameData.fenceValue);		// 同期をとる

	util::ThrowIfFailed(frameData.commandAllocator->Reset());
	
	// コマンド書き込み開始
	mCmdContext.begin(frameData.commandAllocator.Get(), mPipelineState.Get());

	// ルートシグネチャのセット
	mCmdContext.setRootSignature(mRootSignature.Get());

	// ビューポート
	D3D12_VIEWPORT viewport{};
	viewport.Width = 1280;
	viewport.Height = 720;
	mCmdContext.setViewPort(viewport);

	D3D12_RECT scissorRect{};
	scissorRect.right = 1280;
	scissorRect.bottom = 720;
	mCmdContext.setScissorRect(scissorRect);

	// バリアで画面に書き込める状態にする
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// 画面をクリア
	auto handle = mSwapChain.getHandle();
	FLOAT clearColors[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	mCmdContext.clearRenderTarget(handle.cpuHandle, clearColors);

	// 四角描画
	mCmdContext.drawIndxed(6, 1, vbView, ibView);
}

void Renderer::end()
{
	auto& frameData = mFrameDatas[mCurrentFrameIndex];

	// バリアを元の状態に戻す
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドの書き込みを終了
	mCmdContext.end();

	// ため込んだコマンドを実行していく
	ID3D12CommandList* cmdLists[] = { mCmdContext.getCommandLists() };
	mContext.getGraphicsQueue()->ExecuteCommandLists(1, cmdLists);

	// 現在の実行状況を値で取得 (同期をとるために必要)
	frameData.fenceValue = mContext.signalFence();

	// 画面をフリップ
	mSwapChain.present();

	// フレームを次に移動
	mCurrentFrameIndex = (mCurrentFrameIndex + 1) % DeviceContext::MaxFrameCount;
}
