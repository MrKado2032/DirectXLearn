#pragma once
#include "DescriptorAllocator.h"

class D3D12ImGui {
public:
	D3D12ImGui() = default;
	~D3D12ImGui() = default;

	D3D12ImGui(const D3D12ImGui&) = delete;
	D3D12ImGui operator=(const D3D12ImGui&) = delete;

	void initialize(GLFWwindow* window);
	void destroy();

	void begin();
	void end(ID3D12GraphicsCommandList* pCmdList);

private:
	DescriptorAllocator mImguiSrvAllocator;

};