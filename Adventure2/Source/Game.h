//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "Scene.h"
#include "StepTimer.h"
#include "Terrain.h"
#include "TextConsole.h"

// A basic game implementation that creates a D3D12 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify {
 public:
   Game() noexcept(false);
   ~Game();

   Game(Game&&) = default;
   Game& operator=(Game&&) = default;

   Game(Game const&) = delete;
   Game& operator=(Game const&) = delete;

   // Initialization and management
   void Initialize(HWND window, int width, int height);

   // Basic game loop
   void Tick();

   // IDeviceNotify
   void OnDeviceLost() override;
   void OnDeviceRestored() override;

   // Messages
   void OnActivated();
   void OnDeactivated();
   void OnSuspending();
   void OnResuming();
   void OnWindowMoved();
   void OnWindowSizeChanged(int width, int height);

   // Properties
   void GetDefaultSize(int& width, int& height) const noexcept;

 private:
   void Update(DX::StepTimer const& timer);
   void Render();

   void Clear();

   void CreateDeviceDependentResources();
   void CreateWindowSizeDependentResources();

   // Device resources.
   std::unique_ptr<DX::DeviceResources> m_deviceResources;

   // Rendering loop timer.
   DX::StepTimer m_timer;

   // If using the DirectX Tool Kit for DX12, uncomment this line:
   std::unique_ptr<DirectX::GraphicsMemory> m_graphicsMemory;

   std::unique_ptr<DirectX::GamePad> m_gamePad;

   std::unique_ptr<DirectX::Keyboard> m_keyboard;
   std::unique_ptr<DirectX::Mouse> m_mouse;

   std::unique_ptr<TextConsole> m_console;

   std::unique_ptr<DirectX::DescriptorHeap> m_resourceDescriptors;
   std::unique_ptr<DirectX::SpriteFont> m_font;

   std::unique_ptr<Scene> m_scene;

   enum Descriptors
   {
      MyFont,
      Count
   };
};
