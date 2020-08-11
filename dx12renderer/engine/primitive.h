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
      struct DataPerObject {
            DirectX::XMFLOAT4X4 obj2world;
      };
      DataPerObject constant_buffer_cpu;
      static UploadBuffer<DataPerObject> constant_buffer;
public:
      static void initConstBuffer() {
            constant_buffer.Create(g_pd3dDevice, NUM_MAX_PRIMITIVE_3D);
      }
      void init() {
            // update constant buffer (only one globally)
            constant_buffer.CopyData(pIdx, constant_buffer_cpu);
            // create CBV (there's one for each primitive3d), located in a certain place in g_pd3dSrvDescHeap
            uint32_t perObjectByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(DataPerObject));
            uint32_t offset = pIdx * perObjectByteSize;
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
            cbv_desc.BufferLocation = constant_buffer.Resource()->GetGPUVirtualAddress();
            cbv_desc.SizeInBytes = perObjectByteSize;
            SIZE_T constBufferViewSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            D3D12_CPU_DESCRIPTOR_HANDLE hdl = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
            hdl.ptr += constBufferViewSize * (pIdx + 1);
            g_pd3dDevice->CreateConstantBufferView(&cbv_desc, hdl);

            // mesh
            mesh.LoadtoBuffer();
      }
      D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferView() const {
            SIZE_T constBufferViewSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            D3D12_CPU_DESCRIPTOR_HANDLE hdl = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
            hdl.ptr += constBufferViewSize * (pIdx + 1);
            return hdl;
      }
      void updateGPUData() {
            constant_buffer.CopyData(pIdx, constant_buffer_cpu);
      }

public:
      Primitive3D(MeshData&& m) : pIdx(curr_max_pIdx++), mesh(std::move(m)) {
            assert(pIdx <= NUM_MAX_PRIMITIVE_3D - 1);
            init();
      }
};

Primitive3D* make_example_primitive();