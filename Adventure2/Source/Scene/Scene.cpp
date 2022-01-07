#include "pch.h"
#include "Scene.h"

using namespace DirectX;

Scene::Scene(std::unique_ptr<DX::DeviceResources>& deviceResources, std::unique_ptr<ICamera> mainCamera)
    : m_camera(std::move(mainCamera)) {
   auto& loadCommandQueue = deviceResources->GetCopyCommandQueue();
   auto loadCommandList = loadCommandQueue.GetCommandList();

   m_terrain = std::make_unique<Terrain>(loadCommandList.Get(), deviceResources, "Assets\\hmap-16.raw");

   auto fenceValue = loadCommandQueue.ExecuteCommandList(loadCommandList);
   loadCommandQueue.WaitForFenceValue(fenceValue);

   RenderTargetState rtState(deviceResources->GetBackBufferFormat(), deviceResources->GetDepthBufferFormat());

   EffectPipelineStateDescription pd(&VertexType::InputLayout, CommonStates::Opaque, CommonStates::DepthDefault,
                                     CommonStates::Wireframe, rtState);

   m_basicEffect = std::make_unique<BasicEffect>(deviceResources->GetD3DDevice(), EffectFlags::VertexColor, pd);
}

Scene::~Scene() {}

void Scene::Update(float elapsedTime, DirectX::GamePad& gamePad) { m_camera->Update(elapsedTime, gamePad.GetState(0)); }

void Scene::Update(float elapsedTime, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard) {
   m_camera->Update(elapsedTime, mouse, keyboard);
}

void Scene::Render(ID3D12GraphicsCommandList* commandList) {
   m_basicEffect->SetProjection(m_camera->GetProjection());
   m_basicEffect->SetView(m_camera->GetView());
   m_terrain->Render(commandList, m_basicEffect.get());
}
