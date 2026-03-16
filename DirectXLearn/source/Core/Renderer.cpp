#include "pch.h"
#include "Renderer.h"

#include "Util/Helper.h"

#include "D3D12/Mesh.h"

#include "D3D12/GraphicsCore.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::create(uint32_t width, uint32_t height, GLFWwindow* window)
{;
	try {
		auto const& context = GraphicsCore::getDeviceContext();

		mSwapChain.create(context, width, height, glfwGetWin32Window(window));
	
		// コマンドアロケーターの作成
		auto device = context.getDevice();
		for (auto& frame : mFrameDatas) {
			util::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(frame.commandAllocator.GetAddressOf())));
		}

		mCmdContext.create(device, mFrameDatas[0].commandAllocator.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		mCurrentWidth = static_cast<UINT>(width);
		mCurrentHeight = static_cast<UINT>(height);


		// ルートシグネチャの作成 (後にリファクタリング) 
		D3D12_DESCRIPTOR_RANGE1 descriptorRanges[1]{};
		descriptorRanges[0].BaseShaderRegister = 0;
		descriptorRanges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
		descriptorRanges[0].NumDescriptors = 1;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[0].RegisterSpace = 0;

		D3D12_ROOT_PARAMETER1 rootParams[2]{};
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRanges);
		rootParams[0].DescriptorTable.pDescriptorRanges = descriptorRanges;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[1].Constants.ShaderRegister = 0;
		rootParams[1].Constants.RegisterSpace = 0;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		CD3DX12_STATIC_SAMPLER_DESC samplers[1]{};
		samplers->Init(0);

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc{};
		rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSigDesc.Desc_1_1.NumParameters = _countof(rootParams);
		rootSigDesc.Desc_1_1.pParameters = rootParams;
		rootSigDesc.Desc_1_1.NumStaticSamplers = _countof(samplers);
		rootSigDesc.Desc_1_1.pStaticSamplers = samplers;
		rootSigDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> rootSigBinary, errorBinary;
		util::ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootSigDesc, rootSigBinary.GetAddressOf(), errorBinary.GetAddressOf()));

		util::ThrowIfFailed(device->CreateRootSignature(0, rootSigBinary->GetBufferPointer(), rootSigBinary->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));

		// シェーダーのコンパイル
		auto shaderData = shaderLoader.loadShaderFromFile(L"source/Shaders/BasicShader.hlsl");

		// パイプラインステートの作成 (後にリファクタリング)
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.pRootSignature = mRootSignature.Get();
		psoDesc.NodeMask = 0;
		psoDesc.InputLayout = Mesh::getInputLayout();
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

		util::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mPipelineState.GetAddressOf())));

		// ImGuiの初期化
		mImGui.initialize(window);
	}
	catch (std::exception& e) {
		util::Print(e.what());
	}
}

void Renderer::destroy()
{
	// 今ある実行中の処理をすべて待つ
	auto& context = GraphicsCore::getDeviceContext();
	UINT64 completedValue = context.flushGPU();
	for (auto& frame : mFrameDatas) {
		frame.fenceValue = completedValue;
	}

	mImGui.destroy();

	mSwapChain.destroy();
	mCmdContext.destroy();
}

CommandContext& Renderer::begin()
{
	auto& context = GraphicsCore::getDeviceContext();

	auto& frameData = mFrameDatas[mCurrentFrameIndex];	// 現在のフレームデータを取得
	context.waitForFence(frameData.fenceValue);			// 同期をとる

	util::ThrowIfFailed(frameData.commandAllocator->Reset());
	
	// コマンド書き込み開始
	mCmdContext.begin(frameData.commandAllocator.Get(), mPipelineState.Get());

	// ルートシグネチャのセット
	mCmdContext.setRootSignature(mRootSignature.Get());

	// SRVヒープのバインド
	auto& heap = GraphicsCore::getSrvCbvUavAllocator();
	mCmdContext.setDescriptorHeaps(heap.getDescriptorHeap());

	// ビューポート
	D3D12_VIEWPORT viewport{};
	viewport.Width = static_cast<FLOAT>(mCurrentWidth);
	viewport.Height = static_cast<FLOAT>(mCurrentHeight);
	mCmdContext.setViewPort(viewport);

	D3D12_RECT scissorRect{};
	scissorRect.right = static_cast<LONG>(mCurrentWidth);
	scissorRect.bottom = static_cast<LONG>(mCurrentHeight);
	mCmdContext.setScissorRect(scissorRect);

	// バリアで画面に書き込める状態にする
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// ImGuiの描画開始
	mImGui.begin();

	// 画面をクリア
	auto handle = mSwapChain.getHandle();
	FLOAT clearColors[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	mCmdContext.clearRenderTarget(handle.cpuHandle, clearColors);

	return mCmdContext;
}

void Renderer::end()
{
	auto& context = GraphicsCore::getDeviceContext();

	auto& frameData = mFrameDatas[mCurrentFrameIndex];

	// ImGuiの描画終了
	mImGui.end(mCmdContext.getCommandLists());

	// バリアを元の状態に戻す
	mCmdContext.transitionBarrier(mSwapChain.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドの書き込みを終了
	mCmdContext.end();

	// ため込んだコマンドを実行していく
	ID3D12CommandList* cmdLists[] = { mCmdContext.getCommandLists() };
	context.getGraphicsQueue()->ExecuteCommandLists(1, cmdLists);

	// 現在の実行状況を値で取得 (同期をとるために必要)
	frameData.fenceValue = context.signalFence();

	// 画面をフリップ
	mSwapChain.present();

	// フレームを次に移動
	mCurrentFrameIndex = (mCurrentFrameIndex + 1) % DeviceContext::MaxFrameCount;
}

void Renderer::resizeSwapchain(UINT width, UINT height)
{
	mCurrentWidth = width;
	mCurrentHeight = height;
	mSwapChain.resize(width, height);
}
