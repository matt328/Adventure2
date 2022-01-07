#include "pch.h"
#include "Terrain.h"
#include "FileUtils.h"
#include "FreeLookCamera.h"
#include <math.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

Terrain::Terrain(ID3D12GraphicsCommandList* commandList, std::unique_ptr<DX::DeviceResources>& deviceResources,
                 std::vector<VertexType> vertices, std::vector<UINT> indices)
    : m_numIndices(indices.size()), m_originalRotation(Quaternion::Identity) {

   Setup(vertices, indices, deviceResources, commandList);
}

Terrain::Terrain(ID3D12GraphicsCommandList* commandList, std::unique_ptr<DX::DeviceResources>& deviceResources,
                 std::string heightmapFile) {
   auto heightData = ReadRawFile(heightmapFile, 255);

   size_t size = heightData.size();
   auto width = (int)std::sqrtf((float)size);

   std::vector<VertexType> terrainVerts;
   terrainVerts.reserve(size);

   std::vector<UINT> terrainIndices;

   int pos = 0;
   for (int row = 0; row < width; ++row) {
      for (int col = 0; col < width; ++col) {
         float x = (float)row;
         float y = (heightData[pos]);
         float z = (float)col;
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

   m_numIndices = terrainIndices.size();
   Setup(terrainVerts, terrainIndices, deviceResources, commandList);
}

void Terrain::Setup(std::vector<VertexType> vertices, std::vector<UINT> indices,
                    std::unique_ptr<DX::DeviceResources>& deviceResources, ID3D12GraphicsCommandList* commandList) {
   auto device = deviceResources->GetD3DDevice();

   // Upload vertex buffer data
   deviceResources->UpdateBufferResource(commandList, &m_vertexBuffer, &m_intermediateVertexBuffer, vertices.size(),
                                         sizeof(VertexType), &vertices[0], D3D12_RESOURCE_FLAG_NONE);
   m_intermediateVertexBuffer->SetName(L"Intermediate Vertex Buffer");

   // Create the vertex buffer view
   m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
   m_vertexBufferView.SizeInBytes = (UINT)(vertices.size() * sizeof(VertexType));
   m_vertexBufferView.StrideInBytes = sizeof(VertexType);

   // Upload index buffer data
   deviceResources->UpdateBufferResource(commandList, &m_indexBuffer, &m_intermediateIndexBuffer, indices.size(),
                                         sizeof(UINT), &indices[0], D3D12_RESOURCE_FLAG_NONE);
   m_intermediateIndexBuffer->SetName(L"Intermediate Index Buffer");

   // Create the index buffer view
   m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
   m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
   m_indexBufferView.SizeInBytes = (UINT)(indices.size() * sizeof(UINT));

   RenderTargetState rtState(deviceResources->GetBackBufferFormat(), deviceResources->GetDepthBufferFormat());

   EffectPipelineStateDescription pd(&VertexType::InputLayout, CommonStates::Opaque, CommonStates::DepthDefault,
                                     CommonStates::Wireframe, rtState);

   m_effect = std::make_unique<BasicEffect>(device, EffectFlags::VertexColor, pd);

   // Set up matrices
   m_world = Matrix::Identity;

   auto outputSize = deviceResources->GetOutputSize();
   const UINT backBufferWidth = std::max<UINT>(static_cast<UINT>(outputSize.right - outputSize.left), 1u);
   const UINT backBufferHeight = std::max<UINT>(static_cast<UINT>(outputSize.bottom - outputSize.top), 1u);

   m_camera = std::make_unique<FreeLookCamera>(backBufferWidth, backBufferHeight);
}

void Terrain::Update(float elapsedTime, GamePad& gamePad) { m_camera->Update(elapsedTime, gamePad.GetState(0)); }

void Terrain::Update(float elapsedTime, Mouse& mouse, Keyboard& keyboard) {
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
   m_effect->EnableDefaultLighting();
   m_effect->Apply(commandList);

   commandList->DrawIndexedInstanced((UINT)m_numIndices, 1, 0, 0, 0);
}
