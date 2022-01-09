#pragma once

#include "DeviceResources.h"
#include "Effects.h"
#include "ICamera.h"
#include "Terrain.h"

class Scene final {
 public:
   Scene(std::unique_ptr<DX::DeviceResources>& deviceResources, std::unique_ptr<ICamera> mainCamera) noexcept(false);
   ~Scene();

   Scene(Scene&&) = default;
   Scene& operator=(Scene&&) = default;

   Scene(Scene const&) = delete;
   Scene& operator=(Scene const&) = delete;

   void Update(float elapsedTime, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, DirectX::GamePad& gamePad);
   void Render(ID3D12GraphicsCommandList* commandList);

 private:
   std::unique_ptr<ICamera> m_camera;
   std::unique_ptr<Terrain> m_terrain;
   std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
};