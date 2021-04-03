#pragma once
#include "d3dx12.h"
#include "utility.h"
#include "common/common.h"
#include <array>

// Provides basic management of ID3D12Resource
// Includes the facilty of creating resource, updating cpu data, and resource transition
// No CPU-side data is stored. No per-frame facility.
struct Resource {

      ComPtr<ID3D12Resource> _upload_buffer; // for uploading to default buffers
      enum ResourceType {
            UNKNOWN,
            SWAP_CHAIN,         // which heap?
            TEXTURE2D,          // default heap
            VERTEX_IDX_DATA,    // default heap
            RENDER_TARGET,    // can also be texture, default heap
            CONST_BUFFER,     // upload heap
      };
      bool upload_heap;
      ResourceType type;
      D3D12_RESOURCE_FLAGS flags;

      ComPtr<ID3D12Resource> resource;
      D3D12_RESOURCE_STATES curr_state;

      BYTE* mMappedData = nullptr;

      // render target or texture2d
      // NOT used
      /*
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

      }*/
      // General ctor
      Resource(D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) : flags(flags), type(UNKNOWN), curr_state(D3D12_RESOURCE_STATE_GENERIC_READ) {}
      Resource(const Resource& rhs) = delete;
      Resource(Resource&& rhs) : _upload_buffer(std::move(rhs._upload_buffer)), upload_heap(rhs.upload_heap), type(rhs.type),
            flags(rhs.flags), resource(std::move(rhs.resource)), curr_state(rhs.curr_state), mMappedData(rhs.mMappedData) {}
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

      void CreateAs2DTexture(DXGI_FORMAT element_format);

      void CreateFromExistingResource(ID3D12Resource* created, ResourceType type = UNKNOWN, D3D12_RESOURCE_STATES init_state = D3D12_RESOURCE_STATE_COMMON);

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
      D3D12_RESOURCE_DESC resource_desc() const { return resource->GetDesc(); }
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
      UploadBuffer(UploadBuffer&& rhs) : mMappedData(rhs.mMappedData), mElementByteSize(rhs.mElementByteSize), 
            mIsConstantBuffer(rhs.mIsConstantBuffer), mUploadBuffer(std::move(rhs.mUploadBuffer)) {
      }
      UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
      UploadBuffer& operator=(UploadBuffer&& rhs) {
            mUploadBuffer = std::move(rhs.mUploadBuffer);

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
protected:
      bool perframe;
      bool enable_render_target;
      bool enable_texture; // to be determined

      // the cpu descriptor handle in rtv heap
      D3D12_CPU_DESCRIPTOR_HANDLE _cpu_handle_start_rtv; // the first descriptor. Multiple descriptors should be consecutive
      // the cpu descriptor handle in srv heap
      D3D12_CPU_DESCRIPTOR_HANDLE _cpu_handle_start_srv; // the first descriptor
                                                         // the actual gpu resource(s)
      //ID3D12Resource* _resource[NUM_BACK_BUFFERS] = { 0 };
      //Resource _resource[NUM_BACK_BUFFERS];
      std::array<Resource, NUM_BACK_BUFFERS> _resource;
      IDXGISwapChain3* _swapChain = nullptr;

public:
      D3DTexture() = default;
      D3DTexture(const D3DTexture& rhs) = delete;
      D3DTexture(D3DTexture&& rhs) : perframe(rhs.perframe), enable_render_target(rhs.enable_render_target),
            enable_texture(rhs.enable_texture), _cpu_handle_start_rtv(rhs._cpu_handle_start_rtv), _resource(std::move(rhs._resource)) {
            rhs._swapChain = nullptr;
      }
      ~D3DTexture() {
            if (_swapChain) {
                  _swapChain->Release();
            }
      }
      void transit_if_needed(D3D12_RESOURCE_STATES target) {
            if (perframe)
                  _resource[g_frameIndex].transit_if_needed(target);
            else
                  _resource[0].transit_if_needed(target);
      }
      void CreateAsSwapChain(HWND hWnd, DXGI_FORMAT element_format = DXGI_FORMAT_R8G8B8A8_UNORM);
      void CreateAsTexture();
      D3D12_GPU_VIRTUAL_ADDRESS gpu_addr() const {
            if (perframe)
                  return _resource[g_frameIndex].gpu_addr();
            else
                  return _resource[0].gpu_addr();
      }
      // return the handle to the rtv desriptor
      D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle_rtv() const {
            if (perframe) {
                  D3D12_CPU_DESCRIPTOR_HANDLE ret = _cpu_handle_start_rtv;
                  SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                  ret.ptr += rtvDescriptorSize * g_frameIndex;
                  return ret;
            }
            else {
                  return _cpu_handle_start_rtv;
            }
      }

      D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle_srv() const {
            SIZE_T srvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            return D3D12_CPU_DESCRIPTOR_HANDLE();
      }
      D3D12_RESOURCE_DESC resource_desc() const {
            if (perframe)
                  return _resource[g_frameIndex].resource_desc();
            else
                  return _resource[0].resource_desc();
      }
};