#include "pch.h"
#include "Terrain.h"
#include "FreeLookCamera.h"
#include <math.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

Terrain::Terrain(ID3D12GraphicsCommandList* commandList, std::unique_ptr<DX::DeviceResources>& deviceResources,
                 std::vector<VertexType> vertices, std::vector<WORD> indices)
    : m_numIndices(indices.size()), m_originalRotation(Quaternion::Identity) {

   auto device = deviceResources->GetD3DDevice();

   // Upload vertex buffer data
   deviceResources->UpdateBufferResource(commandList,
                                         &m_vertexBuffer,
                                         &m_intermediateVertexBuffer,
                                         vertices.size(),
                                         sizeof(VertexType),
                                         &vertices[0],
                                         D3D12_RESOURCE_FLAG_NONE);
   m_intermediateVertexBuffer->SetName(L"Intermediate Vertex Buffer");

   // Create the vertex buffer view
   m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
   m_vertexBufferView.SizeInBytes = vertices.size() * sizeof(VertexType); // todo check this
   m_vertexBufferView.StrideInBytes = sizeof(VertexType);

   // Upload index buffer data
   deviceResources->UpdateBufferResource(commandList,
                                         &m_indexBuffer,
                                         &m_intermediateIndexBuffer,
                                         indices.size(),
                                         sizeof(WORD),
                                         &indices[0],
                                         D3D12_RESOURCE_FLAG_NONE);
   m_intermediateIndexBuffer->SetName(L"Intermediate Index Buffer");

   // Create the index buffer view
   m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
   m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
   m_indexBufferView.SizeInBytes = indices.size() * sizeof(WORD); // todo check this

   RenderTargetState rtState(deviceResources->GetBackBufferFormat(), deviceResources->GetDepthBufferFormat());

   EffectPipelineStateDescription pd(
       &VertexType::InputLayout, CommonStates::Opaque, CommonStates::DepthDefault, CommonStates::CullNone, rtState);

   m_effect = std::make_unique<BasicEffect>(device, EffectFlags::VertexColor, pd);

   // Set up matrices
   m_world = Matrix::Identity;

   auto outputSize = deviceResources->GetOutputSize();
   const UINT backBufferWidth = std::max<UINT>(static_cast<UINT>(outputSize.right - outputSize.left), 1u);
   const UINT backBufferHeight = std::max<UINT>(static_cast<UINT>(outputSize.bottom - outputSize.top), 1u);

   m_camera = std::make_unique<FreeLookCamera>(backBufferWidth, backBufferHeight);
}

void Terrain::Update(float elapsedTime, Mouse& mouse, Keyboard& keyboard, std::unique_ptr<TextConsole>& console) {
   auto current = Quaternion::CreateFromAxisAngle(ROTATION_AXIS, m_angle);
   auto target = Quaternion::CreateFromAxisAngle(ROTATION_AXIS, m_targetAngle);

   current.Slerp(current, target, elapsedTime);

   m_world = Matrix::CreateFromQuaternion(current);

   m_world *= Matrix::CreateTranslation({0.f, 0.f, 0.f});

   m_camera->Update(elapsedTime, mouse, keyboard);

   m_effect->SetWorld(m_world);
   m_effect->SetProjection(m_camera->GetProjection());
   m_effect->SetView(m_camera->GetView());
}

Terrain::~Terrain() {}

void Terrain::Render(ID3D12GraphicsCommandList* commandList) {
   commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
   commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
   commandList->IASetIndexBuffer(&m_indexBufferView);

   m_effect->Apply(commandList);

   commandList->DrawIndexedInstanced(m_numIndices, 1, 0, 0, 0);
}
