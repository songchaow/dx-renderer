#pragma once
#include "d3dx12.h"
#include "utility.h"
#include "common/common.h"

struct Resource {

      ComPtr<ID3D12Resource> _upload_buffer; // for uploading to default buffers

      UINT id;
      enum ResourceType {
            UNKNOWN,
            TEXTURE2D,          // default heap
            VERTEX_IDX_DATA,    // default heap
            RENDER_TARGET,    // can also be texture, default heap
            CONST_BUFFER,     // upload heap
      };
      bool upload_heap;
      ResourceType type;
      uint32_t depth = 1;
      D3D12_RESOURCE_FLAGS flags;
      D3D12_RESOURCE_DESC desc;
      ComPtr<ID3D12Resource> resource;
      D3D12_RESOURCE_STATES curr_state;

      BYTE* mMappedData = nullptr;

      // render target or texture2d
      // NOT used
      Resource(ResourceType type, UINT width, UINT height, DXGI_FORMAT element_format, UINT depth = 1, UINT16 miplevels = 1,
            UINT sampleCount = 1, UINT sampleQuality = 0)
            : type(type), curr_state(D3D12_RESOURCE_STATE_GENERIC_READ) {
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
      // General ctor
      Resource(D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) : flags(flags), type(UNKNOWN), curr_state(D3D12_RESOURCE_STATE_GENERIC_READ) {}
      // from created resource
      // Resource(D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) {}
      ~Resource() {
            if (type == CONST_BUFFER) {
                  if (resource) {
                        resource->Unmap(0, nullptr);
                        mMappedData = nullptr;
                  }
            }
      }

      void CreateAsConstBuffer(uint32_t buffer_size);

      void CreateAsVertexIdxBuffer(uint32_t buffer_size);

      void _upload_to_default_heap(void* data, uint32_t data_byte_size, uint32_t offset);

      void upload_device_data(void* data, uint32_t data_byte_size) {
            if (upload_heap) {
                  // upload to mMapped directly
                  std::memcpy(mMappedData, data, data_byte_size);
            }
            else {
                  _upload_to_default_heap(data, data_byte_size, 0);// upload to UPLOAD heap first

            }
      }

      void upload_device_data_partial(void* data, uint32_t data_byte_size, uint32_t byte_offset) {
            if (!upload_heap) {
                  std::memcpy(mMappedData + byte_offset, data, data_byte_size);
            }
            else {
                  _upload_to_default_heap(data, data_byte_size, byte_offset);// upload to UPLOAD heap first
            }
      }

      void transit_if_needed(D3D12_RESOURCE_STATES target_state);
      D3D12_GPU_VIRTUAL_ADDRESS gpu_addr() const { return resource->GetGPUVirtualAddress(); }
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

// For now, only supports render target. Add Texture later.
// Multiple buffers may be contained (for each frame), but only one buffer is used when rendering.
class D3DTexture {
      bool perframe;
      bool enable_render_target;
      bool enable_texture; // to be determined

      // the cpu descriptor handle in rtv heap
      D3D12_CPU_DESCRIPTOR_HANDLE _cpu_handle_start; // the first descriptor. Multiple descriptors should be consecutive
      // the actual gpu resource(s)
      D3D12_GPU_VIRTUAL_ADDRESS _resource_addr[NUM_BACK_BUFFERS];

public:
      D3DTexture() = default;

      void CreateAsSwapChain(HWND hWnd);

};