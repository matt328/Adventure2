#pragma once

class UI {
 public:
   static void Init(HWND hWnd, ID3D12Device* pDevice, ID3D12DescriptorHeap* srvDescriptorHeap, DXGI_FORMAT format);
   static void BeginFrame();
   static void EndFrame(ID3D12GraphicsCommandList* pCommandList);
   static void Shutdown();
   static void DrawDemoWindow();
};
