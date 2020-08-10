#pragma once
#include <stdexcept>
#include "wrl/client.h"

#define ThrowIfFailed(f) if( (f) < 0) { throw std::runtime_error("API call failed");}

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class d3dUtil
{
public:
      static UINT CalcConstantBufferByteSize(UINT byteSize)
      {
            // Constant buffers must be a multiple of the minimum hardware
            // allocation size (usually 256 bytes).  So round up to nearest
            // multiple of 256.  We do this by adding 255 and then masking off
            // the lower 2 bytes which store all bits < 256.
            // Example: Suppose byteSize = 300.
            // (300 + 255) & ~255
            // 555 & ~255
            // 0x022B & ~0x00ff
            // 0x022B & 0xff00
            // 0x0200
            // 512
            return (byteSize + 255) & ~255;
      }

      static ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initCPUData, UINT64 byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>&
            uploadBuffer) {

            ComPtr<ID3D12Resource> defaultBuffer;
            ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
                  D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

            // create intermediate buffer
            ThrowIfFailed(device->CreateCommittedResource(
                  &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                  D3D12_HEAP_FLAG_NONE,
                  &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
                  D3D12_RESOURCE_STATE_GENERIC_READ,
                  nullptr,
                  IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

            D3D12_SUBRESOURCE_DATA subResourceData = {};
            subResourceData.pData = initCPUData;
            subResourceData.RowPitch = byteSize;
            subResourceData.SlicePitch = subResourceData.RowPitch;

            cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
            UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
            cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

            return defaultBuffer;
      }

};