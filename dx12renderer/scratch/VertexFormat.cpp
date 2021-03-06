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
      //semantic name   Semantic Index    Format                              InputSlot   ByteOffset  InputSlotClass,                                 d
      {"POSITION",      0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"TEXCOORD",      0,                DXGI_FORMAT_R32G32_FLOAT,           0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"NORMAL",        0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"TANGENT",       0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0}
};

VertexLayoutDesc pbrVertexLayout = { POSITION3F32, TEXCOORD, NORMAL3F32, TANGENT3F32 };

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
      VertexLayoutDesc positionLayout = { POSITION3F32 };
      MeshData ret(positionLayout, (char*)vertex_data_copy, sizeof(vertex_data), (char*)index_data_copy, sizeof(index_data));
      
      return ret;
}

Primitive3D* make_example_primitive() {
      MeshData mesh = make_example_vertexdata();
      Primitive3D* p3D = new Primitive3D(std::move(mesh));
      return p3D;
}

void CreateD3DInputLayoutDesc(VertexLayoutDesc l, std::vector<D3D12_INPUT_ELEMENT_DESC>& d12_desc_data, D3D12_INPUT_LAYOUT_DESC& desc) {
      d12_desc_data.clear();
      for (auto it = l.begin(); it < l.end(); it++) {
            // find previous elements of the same type, and determine semantic index
            uint16_t semanticIdx = 0;
            D3D12_INPUT_ELEMENT_DESC fill_value = vertex_format_descs[*it];
            if (it != l.begin())
                  fill_value.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
            for (auto it_prev = l.begin(); it_prev < it; it_prev++)
                  if (strcmp(vertex_format_descs[*it_prev].SemanticName, vertex_format_descs[*it].SemanticName) == 0) {
                        fill_value.SemanticIndex = vertex_format_descs[*it_prev].SemanticIndex + 1;
                  }
            d12_desc_data.push_back(fill_value);
      }

      desc.pInputElementDescs = d12_desc_data.data();
      desc.NumElements = l.size();

}