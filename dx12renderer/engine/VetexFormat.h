#pragma once
#include <vector>

// an element: point3f, normal3f, etc...
enum ElementFormatName {
      POSITION3F32,
      TEXCOORD,
      NORMAL3F32,
      TANGENT3F32,
      NUM_ELEMENT_FORMAT
};

typedef std::vector<ElementFormatName> VertexLayout;

void CreateD3DInputLayoutDesc(VertexLayout l, std::vector<D3D12_INPUT_ELEMENT_DESC>& element_descs, D3D12_INPUT_LAYOUT_DESC& desc);