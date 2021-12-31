//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "FileUtils.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) {
   m_deviceResources = std::make_unique<DX::DeviceResources>();
   m_deviceResources->RegisterDeviceNotify(this);
   m_console = std::make_unique<TextConsole>();
}

Game::~Game() {
   if (m_deviceResources) { m_deviceResources->WaitForGpu(); }
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height) {
   m_deviceResources->SetWindow(window, width, height);

   m_deviceResources->CreateDeviceResources();
   CreateDeviceDependentResources();

   m_deviceResources->CreateWindowSizeDependentResources();
   CreateWindowSizeDependentResources();

   m_gamePad = std::make_unique<GamePad>();

   m_keyboard = std::make_unique<Keyboard>();
   m_mouse = std::make_unique<Mouse>();

   m_mouse->SetWindow(window);

   // TODO: Change the timer settings if you want something other than the default variable timestep mode.
   // e.g. for 60 FPS fixed timestep update logic, call:
   /*
   m_timer.SetFixedTimeStep(true);
   m_timer.SetTargetElapsedSeconds(1.0 / 60);
   */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick() {
   m_timer.Tick([&]() { Update(m_timer); });

   Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
   PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

   float elapsedTime = float(timer.GetElapsedSeconds());

   m_terrain->Update(elapsedTime, m_mouse->Get(), m_keyboard->Get());

   // TODO: Add your game logic here.
   elapsedTime;

   PIXEndEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render() {
   // Don't try to render anything before the first Update.
   if (m_timer.GetFrameCount() == 0) { return; }

   // Prepare the command list to render a new frame.
   m_deviceResources->Prepare();
   Clear();

   auto commandList = m_deviceResources->GetCommandList();
   PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Render");

   // TODO: Add your rendering code here.
   m_terrain->Render(commandList);

   PIXEndEvent(commandList);

   // Show the new frame.
   PIXBeginEvent(PIX_COLOR_DEFAULT, L"Present");
   m_deviceResources->Present();

   // If using the DirectX Tool Kit for DX12, uncomment this line:
   m_graphicsMemory->Commit(m_deviceResources->GetCommandQueue());

   PIXEndEvent();
}

// Helper method to clear the back buffers.
void Game::Clear() {
   auto commandList = m_deviceResources->GetCommandList();
   PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

   // Clear the views.
   auto rtvDescriptor = m_deviceResources->GetRenderTargetView();
   auto dsvDescriptor = m_deviceResources->GetDepthStencilView();

   commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
   commandList->ClearRenderTargetView(rtvDescriptor, Colors::CornflowerBlue, 0, nullptr);
   commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

   // Set the viewport and scissor rect.
   auto viewport = m_deviceResources->GetScreenViewport();
   auto scissorRect = m_deviceResources->GetScissorRect();
   commandList->RSSetViewports(1, &viewport);
   commandList->RSSetScissorRects(1, &scissorRect);

   PIXEndEvent(commandList);
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated() {
   // TODO: Game is becoming active window.
   m_gamePad->Resume();
}

void Game::OnDeactivated() {
   // TODO: Game is becoming background window.
   m_gamePad->Suspend();
}

void Game::OnSuspending() {
   // TODO: Game is being power-suspended (or minimized).
   m_gamePad->Suspend();
}

void Game::OnResuming() {
   m_timer.ResetElapsedTime();
   m_gamePad->Resume();
   // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved() {
   auto r = m_deviceResources->GetOutputSize();
   m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height) {
   if (!m_deviceResources->WindowSizeChanged(width, height)) return;

   CreateWindowSizeDependentResources();

   // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
   // TODO: Change to desired default window size (note minimum size is 320x200).
   width = 800;
   height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources() {
   auto device = m_deviceResources->GetD3DDevice();

   // Check Shader Model 6 support
   D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {D3D_SHADER_MODEL_6_0};
   if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel))) ||
       (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0)) {
#ifdef _DEBUG
      OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
      throw std::runtime_error("Shader Model 6.0 is not supported!");
   }

   // If using the DirectX Tool Kit for DX12, uncomment this line:
   m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

   // TODO: Initialize device dependent objects here (independent of window size).

   m_resourceDescriptors = std::make_unique<DescriptorHeap>(device, Descriptors::Count);

   auto resourceUpload = ResourceUploadBatch(device);
   resourceUpload.Begin();

   RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat());

   auto uploadResourcesFinished = resourceUpload.End(m_deviceResources->GetCommandQueue());

   uploadResourcesFinished.wait();

   auto heightData = ReadRawFile<char>("Assets//2x2.raw");
   size_t size = heightData.size();
   auto width = (int)std::sqrtf((float)size);

   std::vector<VertexType> terrainVerts;
   terrainVerts.reserve(size);

   std::vector<UINT> terrainIndices;

   int pos = 0;
   for (int row = 0; row < width; ++row) {
      for (int col = 0; col < width; ++col) {
         float x = (float)row;
         float y = (float)(heightData[pos]);
         float z = (float)col;
         auto color = Vector4{};
         switch (pos) {
            case 0: // Red
               color.x = 1.f;
               break;
            case 1: // Green
               color.y = 1.f;
               break;
            case 2: // Blue
               color.z = 1.f;
               break;
            default: // White
               color = Vector4::One;
         }
         terrainVerts.push_back({Vector3(x, y, z), Vector3(0.f, 1.f, 0.f)});
         pos++;
      }
   }

   for (int y = 0; y < width - 1; ++y) {
      for (int x = 0; x < width - 1; ++x) {
         UINT start = y * width + x;
         terrainIndices.push_back(start);
         terrainIndices.push_back(start + 1);
         terrainIndices.push_back(start + width);

         terrainIndices.push_back(start + 1);
         terrainIndices.push_back(start + 1 + width);
         terrainIndices.push_back(start + width);
      }
   }

   std::vector<VertexType> vertices = {
       {Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)}, // 0
       {Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f)},  // 1
       {Vector3(1.0f, 1.0f, -1.0f), Vector3(1.0f, 1.0f, 0.0f)},   // 2
       {Vector3(1.0f, -1.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f)},  // 3
       {Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)},  // 4
       {Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f)},   // 5
       {Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f)},    // 6
       {Vector3(1.0f, -1.0f, 1.0f), Vector3(1.0f, 0.0f, 1.0f)}    // 7
   };

   std::vector<UINT> indices = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 4, 5, 1, 4, 1, 0,
                                3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7};

   auto& loadCommandQueue = m_deviceResources->GetCopyCommandQueue();
   auto loadCommandList = loadCommandQueue.GetCommandList();

   m_terrain = std::make_unique<Terrain>(loadCommandList.Get(), m_deviceResources, vertices, indices);

   auto fenceValue = loadCommandQueue.ExecuteCommandList(loadCommandList);
   loadCommandQueue.WaitForFenceValue(fenceValue);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources() {
   // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost() {
   // TODO: Add Direct3D resource cleanup here.

   // If using the DirectX Tool Kit for DX12, uncomment this line:
   m_graphicsMemory.reset();
   m_terrain.reset();
}

void Game::OnDeviceRestored() {
   CreateDeviceDependentResources();

   CreateWindowSizeDependentResources();
}
#pragma endregion
