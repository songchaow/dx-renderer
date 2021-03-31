#pragma once
#include "d3dx12.h"
#include "utility.h"
#include "d3dbootstrap.h"

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
      UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
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

private:
      Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
      BYTE* mMappedData = nullptr;

      UINT mElementByteSize = 0;
      bool mIsConstantBuffer = false;
};


// Constant buffer
template<typename T>
class CBuffer {
      std::vector<T> data_cpu;
      std::vector<UploadBuffer<T>> cbuffers; // GPU cbuffers in each frame
      int dirty_count = 0;
public:
      CBuffer(ID3D12Device* device, UINT elementCount, bool perFrame = true) {
            if (perFrame) {
                  for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
                        cbuffers.push_back(UploadBuffer<T>(device, elementCount, true));
            }
            else
                  cbuffers.push_back(UploadBuffer<T>(device, elementCount, true));
            cbuffers.shrink_to_fit();

            data_cpu.resize(elementCount);
            data_cpu.shrink_to_fit();
      }
      CBuffer(ID3D12Device* device, bool perFrame = true) : CBuffer(device, 1, perFrame) {}

      void update_cpu_buffer(int eidx, T* data) {
            data_cpu[eidx] = *data;
            dirty_count = cbuffers.size();
      }

      void update_cpu_buffer_single(T* data) { update_cpu_buffer(0, data); }

      void updateGPUAll(int eidx, T* data) {
            for (auto &c : cbuffers) {
                  c.CopyData(eidx, *data);
            }
            dirty_count = 0;
      }
      void updateGPUCurrFrame(int eidx, T* data) {
            if (cbuffers.size() > 1) {
                  cbuffers[g_frameIndex].CopyData(eidx, *data);
            }
            if (dirty_count > 0)
                  --dirty_count;
      }

      bool dirty() const { return dirty_count > 0; }
};