#pragma once
#include "DeviceResources.h"

class ICamera;
class TextConsole;
namespace DirectX {
   class BasicEffect;
}

using VertexType = DirectX::VertexPositionColor;

constexpr DirectX::SimpleMath::Vector3 ROTATION_AXIS = DirectX::SimpleMath::Vector3(0, 1, 0);
constexpr DirectX::SimpleMath::Vector3 EYE_POSITION = DirectX::SimpleMath::Vector3(0, 0, -10);
constexpr DirectX::SimpleMath::Vector3 LOOK_AT = DirectX::SimpleMath::Vector3(0, 0, 0);
constexpr DirectX::SimpleMath::Vector3 UP = DirectX::SimpleMath::Vector3(0, 1, 0);

class Terrain {
 public:
   Terrain(ID3D12GraphicsCommandList* commandList, std::unique_ptr<DX::DeviceResources>& deviceResources,
           std::vector<VertexType> vertices, std::vector<UINT> indices);

   Terrain(ID3D12GraphicsCommandList* commandList, std::unique_ptr<DX::DeviceResources>& deviceResources,
           std::string heightmapFile);

   ~Terrain();

   void Render(ID3D12GraphicsCommandList* commandList, DirectX::BasicEffect* effect);

 private:
   void Setup(std::vector<VertexType> vertices, std::vector<UINT> indices,
              std::unique_ptr<DX::DeviceResources>& deviceResources, ID3D12GraphicsCommandList* commandList);

   std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_intermediateVertexBuffer;
   Microsoft::WRL::ComPtr<ID3D12Resource> m_intermediateIndexBuffer;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
   D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
   D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

   size_t m_numIndices;

   DirectX::SimpleMath::Matrix m_world;
};
