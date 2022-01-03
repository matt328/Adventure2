#include "pch.h"
#include "Scene.h"

Scene::Scene(std::unique_ptr<DX::DeviceResources>& deviceResources) {
   auto& loadCommandQueue = deviceResources->GetCopyCommandQueue();
   auto loadCommandList = loadCommandQueue.GetCommandList();

   m_terrain = std::make_unique<Terrain>(loadCommandList.Get(), deviceResources, "Assets\\hmap-16.raw");

   auto fenceValue = loadCommandQueue.ExecuteCommandList(loadCommandList);
   loadCommandQueue.WaitForFenceValue(fenceValue);
}

Scene::~Scene() {}

void Scene::Update(float elapsedTime, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard) {
   m_terrain->Update(elapsedTime, mouse, keyboard);
}

void Scene::Render(ID3D12GraphicsCommandList* commandList) { m_terrain->Render(commandList); }
