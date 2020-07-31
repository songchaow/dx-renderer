#include "d3dbootstrap.h"
#include <vector>

// an element: point3f, normal3f, etc...
enum ElementFormatName {
      POSITION3F32,
      TEXCOORD,
      NORMAL3F32,
      TANGENT3F32,
      NUM_ELEMENT_FORMAT
};

// seems only name and format are determined
D3D12_INPUT_ELEMENT_DESC vertex_format_descs[NUM_ELEMENT_FORMAT] = {
      //name            Semantic Index    Format                              InputSlot   ByteOffset  InputSlotClass
      {"POSITION",      0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"TEXCOORD",      0,                DXGI_FORMAT_R32G32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"NORMAL",      0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0},
      {"TANGENT",      0,                DXGI_FORMAT_R32G32B32A32_FLOAT,     0,          0,          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0}
};

typedef std::vector<ElementFormatName> VertexLayout;

VertexLayout pbrVertexLayout = { POSITION3F32, TEXCOORD, NORMAL3F32, TANGENT3F32 };

void CreateD3DInputLayoutDesc(VertexLayout l, std::vector<D3D12_INPUT_ELEMENT_DESC>& element_descs) {
      element_descs.clear();
      for (auto& d : l) {
            element_descs.push_back(vertex_format_descs[d]);
      }
      D3D12_INPUT_LAYOUT_DESC desc;
      desc.pInputElementDescs = element_descs.data();
      desc.NumElements = l.size();

}