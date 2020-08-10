#include "d3dbootstrap.h"
#include <vector>
#include <memory>
#include "wrl/client.h"

#include "d3dx12.h"
#include "utility/utility.h"

#include "engine/primitive.h"

extern ID3D12Device* g_pd3dDevice;
extern ID3D12GraphicsCommandList*   g_pd3dCommandList;

uint32_t element_byte_Length[NUM_ELEMENT_FORMAT] = {
      12,
      8,
      12,
      12
};
// seems only name and format are determined
D3D12_INPUT_ELEMENT_DESC vertex_format_descs[NUM_ELEMENT_FORMAT] = {
      //semantic name   Semantic Index    Format                              InputSlot   ByteOffset  InputSlotClass
      {"POSITION",      0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"TEXCOORD",      0,                DXGI_FORMAT_R32G32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"NORMAL",        0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"TANGENT",       0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0}
};

VertexLayout pbrVertexLayout = { POSITION3F32, TEXCOORD, NORMAL3F32, TANGENT3F32 };

ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initCPUData, UINT64 byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>&
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

MeshData make_example_vertexdata() {
      constexpr uint8_t vertexNum = 8;
      static float vertex_data[vertexNum * 3] = {
            -1.0, -1.0, -1.0,
            -1.0, -1.0, 1.0,
            -1.0, 1.0, -1.0,
            -1.0, 1.0, 1.0,
             1.0, -1.0, -1.0,
             1.0, -1.0, 1.0,
             1.0, 1.0, -1.0,
             1.0, 1.0, 1.0
      };
      float* vertex_data_copy = new float[vertexNum * 3];
      std::memcpy(vertex_data_copy, vertex_data, sizeof(float) * vertexNum * 3);
      static float index_data[36] = {
            //front
            0,1,2,
            0,2,3,
            //back
            4,6,5,
            4,7,6,
            //left
            4,5,1,
            4,1,0,
            //right
            3,2,6,
            3,6,7,
            //top
            1,5,6,
            1,6,2,
            //bottom
            4,0,3,
            4,3,7,
      };
      uint16_t* index_data_copy = new uint16_t[36];
      std::memcpy(index_data_copy, index_data, sizeof(uint16_t) * 36);
      VertexLayout positionLayout = { POSITION3F32 };
      MeshData ret(positionLayout, (char*)vertex_data_copy, sizeof(vertex_data), (char*)index_data_copy, sizeof(index_data));
      
      return ret;
}

Primitive3D* make_example_primitive() {
      Primitive3D* p3D = new Primitive3D(;
}

void CreateD3DInputLayoutDesc(VertexLayout l, std::vector<D3D12_INPUT_ELEMENT_DESC>& element_descs) {
      element_descs.clear();
      for (auto& d : l) {
            element_descs.push_back(vertex_format_descs[d]);
      }
      D3D12_INPUT_LAYOUT_DESC desc;
      desc.pInputElementDescs = element_descs.data();
      desc.NumElements = l.size();

}