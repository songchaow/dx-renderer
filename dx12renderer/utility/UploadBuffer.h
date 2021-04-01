#pragma once
#include "d3dx12.h"
#include "utility.h"

struct Resource {
      UINT id;
      enum ResourceType {
            TEXTURE2D,          // default heap
            RENDER_TARGET,    // can also be texture, default heap
            CONST_BUFFER,     // upload heap
      };
      ResourceType type;
      //union ResourceSize {
      //      Point2i size2d; // element counts, NOT byte size
      //      uint32_t length; // byte length?
      //};
      //ResourceSize size;
      uint32_t depth = 1;
      D3D12_RESOURCE_DESC desc;
      ComPtr<ID3D12Resource> resource;
      D3D12_RESOURCE_STATES curr_state;
      void Create();

      // render target or texture2d
      Resource(ResourceType type, UINT width, UINT height, DXGI_FORMAT element_format, UINT depth = 1, UINT16 miplevels = 1,
            D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_GENERIC_READ, UINT sampleCount = 1, UINT sampleQuality = 0)
            : type(type), curr_state(initialState) {
            desc.Alignment = 0;
            desc.MipLevels = miplevels;
            desc.Format = element_format;
            desc.SampleDesc.Count = sampleCount;
            desc.SampleDesc.Quality = sampleQuality;
            desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            
            desc.Width = width; desc.Height = height; desc.DepthOrArraySize = depth;
            if (type == RENDER_TARGET)
                  desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

      }

      void transit_if_needed(D3D12_RESOURCE_STATES target_state);
};

// A buffer which resides on the upload heap and provides upload facility.
template<typename T>
class UploadBuffer
{
public:
      UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :
            mIsConstantBuffer(isConstantBuffer)
      {
            Create(device, elementCount);
      }
      UploadBuffer() = default;
      void Create(ID3D12Device* device, UINT elementCount) {
            mElementByteSize = sizeof(T);

            // Constant buffer elements need to be multiples of 256 bytes.
            // This is because the hardware can only view constant data 
            // at m*256 byte offsets and of n*256 byte lengths. 
            // typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
            // UINT64 OffsetInBytes; // multiple of 256
            // UINT   SizeInBytes;   // multiple of 256
            // } D3D12_CONSTANT_BUFFER_VIEW_DESC;
            if (mIsConstantBuffer)
                  mElementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));

            ThrowIfFailed(device->CreateCommittedResource(
                  &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                  D3D12_HEAP_FLAG_NONE,
                  &CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
                  D3D12_RESOURCE_STATE_GENERIC_READ,
                  nullptr,
                  IID_PPV_ARGS(&mUploadBuffer)));

            ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
            // We do not need to unmap until we are done with the resource.  However, we must not write to
            // the resource while it is in use by the GPU (so we must use synchronization techniques).
      }

      UploadBuffer(const UploadBuffer& rhs) = delete;
      UploadBuffer(UploadBuffer&& rhs) {

      }
      UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
      UploadBuffer& operator=(UploadBuffer&& rhs) {
            mUploadBuffer = rhs.mUploadBuffer;
            rhs.mUploadBuffer.Reset();

            mMappedData = rhs.mMappedData;
            mElementByteSize = rhs.mElementByteSize;
            mIsConstantBuffer = rhs.mIsConstantBuffer;
      }
      ~UploadBuffer()
      {
            if (mUploadBuffer != nullptr)
                  mUploadBuffer->Unmap(0, nullptr);

            mMappedData = nullptr;
      }

      ID3D12Resource* Resource()const
      {
            return mUploadBuffer.Get();
      }

      void CopyData(int elementIndex, const T& data)
      {
            memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
      }

protected:
      Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
      BYTE* mMappedData = nullptr;

      UINT mElementByteSize = 0;
      bool mIsConstantBuffer = false;
};
