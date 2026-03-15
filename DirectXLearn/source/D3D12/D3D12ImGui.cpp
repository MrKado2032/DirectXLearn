#include "pch.h"
#include "D3D12ImGui.h"

#include "GraphicsCore.h"

#include "DX12-Imgui/imgui_impl_dx12.h"
#include "DX12-Imgui/imgui_impl_glfw.h"

void D3D12ImGui::initialize(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOther(window, true);

	auto& context = GraphicsCore::getDeviceContext();

	mImguiSrvAllocator.create(context.getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, DeviceContext::MaxFrameCount, true);
	auto fontDesc = mImguiSrvAllocator.allocate();

	ImGui_ImplDX12_Init(context.getDevice(), DeviceContext::MaxFrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, mImguiSrvAllocator.getDescriptorHeap(), fontDesc.cpuHandle, fontDesc.gpuHandle);

	// --- 追加: フォントアトラスを明示的にビルドする ---
	unsigned char* pixels;
	int width, height;
	ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	// ----------------------------------------------
}

void D3D12ImGui::destroy()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void D3D12ImGui::begin()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void D3D12ImGui::end(ID3D12GraphicsCommandList* pCmdList)
{
	ImGui::Render();

	ID3D12DescriptorHeap* ppHeaps[] = { mImguiSrvAllocator.getDescriptorHeap() };
	pCmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCmdList);
}
