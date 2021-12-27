#pragma once

#include "pch.h"

class TextConsole {
 public:
   TextConsole();
   TextConsole(ID3D12Device* device, DirectX::ResourceUploadBatch& upload, const DirectX::RenderTargetState& rtState,
               const wchar_t* fontName, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor,
               D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor);

   TextConsole(TextConsole&&) = delete;
   TextConsole& operator=(TextConsole&&) = delete;

   TextConsole(TextConsole const&) = delete;
   TextConsole& operator=(TextConsole const&) = delete;

   void Render(ID3D12GraphicsCommandList* commandList);

   void Clear();

   void Write(const wchar_t* str);
   void WriteLine(const wchar_t* str);
   void Format(const wchar_t* strFormat, ...);

   void SetWindow(const RECT& layout);

   void XM_CALLCONV SetForegroundColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat4(&m_textColor, color); }

   void ReleaseDevice();
   void RestoreDevice(ID3D12Device* device, DirectX::ResourceUploadBatch& upload,
                      const DirectX::RenderTargetState& rtState, const wchar_t* fontName,
                      D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor);

   void SetViewport(const D3D12_VIEWPORT& viewPort);

   void SetRotation(DXGI_MODE_ROTATION rotation);

 private:
   void ProcessString(const wchar_t* str);
   void IncrementLine();

   RECT m_layout;
   DirectX::XMFLOAT4 m_textColor;

   unsigned int m_columns;
   unsigned int m_rows;
   unsigned int m_currentColumn;
   unsigned int m_currentLine;

   std::unique_ptr<wchar_t[]> m_buffer;
   std::unique_ptr<wchar_t*[]> m_lines;
   std::vector<wchar_t> m_tempBuffer;

   std::unique_ptr<DirectX::SpriteBatch> m_batch;
   std::unique_ptr<DirectX::SpriteFont> m_font;

   std::mutex m_mutex;
};
