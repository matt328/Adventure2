#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <queue>

namespace DX
{

   class CommandQueue
   {
    public:
      CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
      virtual ~CommandQueue();

      // Get an available command list from the command queue.
      Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList();

      // Execute a command list.
      // Returns the fence value to wait for for this command list.
      uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

      uint64_t Signal();
      bool IsFenceComplete(uint64_t fenceValue);
      void WaitForFenceValue(uint64_t fenceValue);
      void Flush();

      Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

    protected:
      Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
      Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
          Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

    private:
      // Keep track of command allocators that are "in-flight"
      struct CommandAllocatorEntry
      {
         uint64_t fenceValue;
         Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
      };

      using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>;
      using CommandListQueue = std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>;

      D3D12_COMMAND_LIST_TYPE m_commandListType;
      Microsoft::WRL::ComPtr<ID3D12Device> m_device;
      Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
      Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
      HANDLE m_fenceEvent;
      uint64_t m_fenceValue;

      CommandAllocatorQueue m_commandAllocatorQueue;
      CommandListQueue m_commandListQueue;
   };
} // namespace DX