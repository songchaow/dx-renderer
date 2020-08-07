#pragma once
#include "d3dx12.h"
#include "stdafx.h"
#include "utility/UploadBuffer.h"
#include "d3dbootstrap.h"
#include <cassert>
#include "utility/utility.h"



class Primitive3D {
public:
      static constexpr uint32_t NUM_MAX_PRIMITIVE_3D = 200;
private:
      static uint32_t curr_max_pIdx;
      uint32_t pIdx;
      // constant buffer content
      struct DataPerObject {
            DirectX::XMFLOAT4X4 obj2world;
      };
      DataPerObject constant_buffer_cpu;
      static UploadBuffer<DataPerObject> constant_buffer;
      static void initConstBuffer() {
            constant_buffer.Create(g_pd3dDevice, NUM_MAX_PRIMITIVE_3D);
      }
      void init() {
            // update constant buffer (only one globally)
            constant_buffer.CopyData(pIdx, constant_buffer_cpu);
            // create CBV (there's one for each primitive3d)
            uint32_t perObjectByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(DataPerObject));
            uint32_t offset = pIdx * perObjectByteSize;
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
            cbv_desc.BufferLocation = constant_buffer.Resource()->GetGPUVirtualAddress();
            cbv_desc.SizeInBytes = perObjectByteSize;
            SIZE_T constBufferViewSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            D3D12_CPU_DESCRIPTOR_HANDLE hdl = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
            hdl.ptr += constBufferViewSize * pIdx;
            g_pd3dDevice->CreateConstantBufferView(&cbv_desc, hdl);

      }
      void updateGPUData() {
            constant_buffer.CopyData(pIdx, constant_buffer_cpu);
      }

public:
      Primitive3D() : pIdx(curr_max_pIdx++) {
            assert(pIdx <= NUM_MAX_PRIMITIVE_3D - 1);
      }
};