#pragma once
#include "utility/UploadBuffer.h"
#include "d3dbootstrap.h"

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

      void updateGPUAll(int eidx) {
            for (auto &c : cbuffers) {
                  c.CopyData(eidx, data_cpu[eidx]);
            }
            dirty_count = 0;
      }
      void updateGPUCurrFrame(int eidx) {
            if (cbuffers.size() > 1) {
                  cbuffers[g_frameIndex].CopyData(eidx, data_cpu[eidx]);
            }
            if (dirty_count > 0)
                  --dirty_count;
      }
      void update_gpu_frame_if_needed(int edix) {
            if (dirty_count > 0)
                  updateGPUCurrFrame(edix);
      }

      bool dirty() const { return dirty_count > 0; }
      D3D12_GPU_VIRTUAL_ADDRESS gpu_addr_curr_frame(int eidx) {
            UINT64 perObjectByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(DataPerPrimitive3D));
            UINT64 offset = eidx * perObjectByteSize;

            D3D12_GPU_VIRTUAL_ADDRESS startAddr;
            if (cbuffers.size() > 1)
                  startAddr = cbuffers[g_frameIndex].Resource()->GetGPUVirtualAddress();
            else
                  startAddr = cbuffers[0].Resource()->GetGPUVirtualAddress();
            return startAddr + offset;
      }
};
