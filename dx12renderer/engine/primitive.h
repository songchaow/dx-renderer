#pragma once
#include "d3dx12.h"
#include "stdafx.h"
#include "utility/UploadBuffer.h"
#include "d3dbootstrap.h"
#include <cassert>
#include "utility/utility.h"
#include "engine/VetexFormat.h"

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

extern uint32_t element_byte_Length[NUM_ELEMENT_FORMAT];

struct MeshData {
      VertexLayout _layout; // initialize
      std::unique_ptr<char[]> _dataVertex; // initialize
      std::unique_ptr<char[]> _dataIndex; // initialize

      ComPtr<ID3D12Resource> uploadBufferVertex;
      ComPtr<ID3D12Resource> uploadBufferIndex;

      ComPtr<ID3D12Resource> vertexBuffer = nullptr;
      ComPtr<ID3D12Resource> indexBuffer = nullptr;
      D3D12_VERTEX_BUFFER_VIEW vbv;
      D3D12_INDEX_BUFFER_VIEW ibv;
      uint64_t vertexNum; // initialize
      uint64_t indexNum; // initialize
      DXGI_FORMAT indexFormat; // initialize (maybe from byte stride)

      uint64_t byteLength() {
            uint64_t len = 0;
            for (auto& l : _layout)
                  len += element_byte_Length[l];
            return len * vertexNum;
      }
      uint64_t indexByteLength() {
            constexpr uint64_t preIndexByteLength = 4;
            return indexNum * preIndexByteLength;
      }
      uint64_t byteStride() {
            uint64_t len = 0;
            for (auto& l : _layout)
                  len += element_byte_Length[l];
            return len;
      }
      void LoadtoBuffer() {
            if (!_dataVertex)
                  return;
            // create vertex buffer

            vertexBuffer = d3dUtil::CreateDefaultBuffer(g_pd3dDevice, g_pd3dCommandList, _dataVertex.get(), byteLength(), uploadBufferVertex);
            // create VBV
            vbv.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
            vbv.SizeInBytes = byteLength();
            vbv.StrideInBytes = byteStride();

            // (used when draw) bound to pipeline (set input slot and view)
            // g_pd3dCommandList->IASetVertexBuffers(0, 1, &vbv);

            // create index buffer
            // reuse uploadBuffer? no
            indexBuffer = d3dUtil::CreateDefaultBuffer(g_pd3dDevice, g_pd3dCommandList, _dataIndex.get(), indexByteLength(), uploadBufferIndex);
            // create IBV
            ibv.BufferLocation = indexBuffer->GetGPUVirtualAddress();
            ibv.SizeInBytes = indexByteLength();
            ibv.Format = indexFormat;

            // (used when draw) bound to pipeline
            // g_pd3dCommandList->IASetIndexBuffer(&ibv);
      }

      MeshData(VertexLayout l, char* dataVertex, uint64_t numVertex, char* dataIndex, uint64_t numIndex) : _layout(l), _dataVertex(dataVertex),
            _dataIndex(dataIndex), vertexNum(numVertex), indexNum(numIndex), indexFormat(DXGI_FORMAT_R16_UINT) {}
      MeshData() : vertexNum(0), indexNum(0) {}
      MeshData(MeshData&& m) : _layout(m._layout), _dataVertex(std::move(m._dataVertex)), _dataIndex(std::move(m._dataIndex)), vertexBuffer(m.vertexBuffer),
            indexBuffer(m.indexBuffer), vbv(m.vbv), ibv(m.ibv), vertexNum(m.vertexNum), indexNum(m.indexNum), indexFormat(m.indexFormat) {}
};

class Primitive3D {
public:
      static constexpr uint32_t NUM_MAX_PRIMITIVE_3D = 200;
      
private:
      // pIdx
      static uint32_t curr_max_pIdx;
      uint32_t pIdx;
      // mesh
      MeshData mesh;
      // constant buffer content
      
      DataPerPrimitive3D constant_buffer_cpu;
      bool constant_buffer_dirty = false;
      //static UploadBuffer<DataPerPrimitive3D> constant_buffer;
public:
      // init the continuous constant buffer as a whole , for all primitives
      static void initConstBuffer() {
            // init for each frame resource
            for (uint32_t i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
                  g_frameContext[i].constant_buffer_primitive3d.Create(g_pd3dDevice, NUM_MAX_PRIMITIVE_3D);
      }
      void init() {
            uint32_t perObjectByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(DataPerPrimitive3D));
            uint32_t offset = pIdx * perObjectByteSize;
            for (uint32_t idx = 0; idx < NUM_FRAMES_IN_FLIGHT; idx++) {
                  // update constant buffer on every frame
                  g_frameContext[g_frameIndex].constant_buffer_primitive3d.CopyData(pIdx, constant_buffer_cpu);
            }
            #if 0
            // create CBV (there's one for each primitive3d) on each frame resource, located in a certain place in g_pd3dSrvDescHeap
            for (uint32_t idx = 0; idx < NUM_FRAMES_IN_FLIGHT; idx++) {
                  // update constant buffer on every frame
                  g_frameContext[g_frameIndex].constant_buffer_primitive3d.CopyData(pIdx, constant_buffer_cpu);
                  D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
                  cbv_desc.BufferLocation = g_frameContext[g_frameIndex].constant_buffer_primitive3d.Resource()->GetGPUVirtualAddress();
                  cbv_desc.SizeInBytes = perObjectByteSize;
                  SIZE_T constBufferViewSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                  D3D12_CPU_DESCRIPTOR_HANDLE hdl = ConstantBufferViewCurrFrame();
                  g_pd3dDevice->CreateConstantBufferView(&cbv_desc, hdl); // the view need to be created frequently! no
            }
            #endif

            // mesh
            mesh.LoadtoBuffer();
      }
      D3D12_GPU_VIRTUAL_ADDRESS ConstBufferGPUAddressCurrFrame() const {
            UINT64 perObjectByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(DataPerPrimitive3D));
            UINT64 offset = pIdx * perObjectByteSize;
            D3D12_GPU_VIRTUAL_ADDRESS startAddr = g_frameContext[g_frameIndex].constant_buffer_primitive3d.Resource()->GetGPUVirtualAddress();
            return startAddr + offset;
      }
      #if 0
      D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferViewCurrFrame() const {
            SIZE_T constBufferViewSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            D3D12_CPU_DESCRIPTOR_HANDLE hdl = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
            hdl.ptr += constBufferViewSize * (pIdx + (SIZE_T)CBVLocation::PER_OBJECT) + NUM_MAX_PRIMITIVE_3D * constBufferViewSize * g_frameIndex;
            return hdl;
      }
      D3D12_GPU_DESCRIPTOR_HANDLE ConstantBufferViewGPU() const {
            SIZE_T constBufferViewSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            D3D12_GPU_DESCRIPTOR_HANDLE hdl = g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
            hdl.ptr += constBufferViewSize * (pIdx + (SIZE_T)CBVLocation::PER_OBJECT) + NUM_MAX_PRIMITIVE_3D * constBufferViewSize * g_frameIndex;
            return hdl;
      }
      D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferView(uint32_t frameIdx) const {
            return D3D12_CPU_DESCRIPTOR_HANDLE();
      }
      #endif
      void updateGPUData() {
            g_frameContext[g_frameIndex].constant_buffer_primitive3d.CopyData(pIdx, constant_buffer_cpu);

      }

public:
      Primitive3D(MeshData&& m) : pIdx(curr_max_pIdx++), mesh(std::move(m)) {
            assert(pIdx <= NUM_MAX_PRIMITIVE_3D - 1);
            init();
      }
};

Primitive3D* make_example_primitive();