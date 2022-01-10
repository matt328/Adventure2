#include "pch.h"

#include "UI.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>

using namespace Microsoft::WRL;

void UI::Init(HWND hWnd, ID3D12Device* pDevice, ID3D12DescriptorHeap* srvDescriptorHeap, DXGI_FORMAT format) {
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
   ImGui::StyleColorsDark();

   ImGui_ImplWin32_Init(hWnd);
   ImGui_ImplDX12_Init(pDevice,
                       2,
                       format,
                       srvDescriptorHeap,
                       srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                       srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

   io.Fonts->AddFontDefault();
}

void UI::BeginFrame() {
   ImGui_ImplDX12_NewFrame();
   ImGui_ImplWin32_NewFrame();
   ImGui::NewFrame();
}

void UI::EndFrame(ID3D12GraphicsCommandList* pCommandList) {
   ImGui::Render();
   ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
}

void UI::Shutdown() {
   ImGui_ImplDX12_Shutdown();
   ImGui_ImplWin32_Shutdown();
   ImGui::DestroyContext();
}

void UI::DrawDemoWindow() {
   ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
   static float f = 0.0f;
   static int counter = 0;

   ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

   ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

   ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
   ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

   if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
   ImGui::SameLine();
   ImGui::Text("counter = %d", counter);

   ImGui::Text(
       "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
   ImGui::End();
}
