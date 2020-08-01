#include "d3dbootstrap.h"
#include <vector>
#include <memory>
#include "wrl/client.h"

#include "d3dx12.h"
#include "utility/utility.h"

// an element: point3f, normal3f, etc...
enum ElementFormatName {
      POSITION3F32,
      TEXCOORD,
      NORMAL3F32,
      TANGENT3F32,
      NUM_ELEMENT_FORMAT
};

static uint32_t byte_Length[NUM_ELEMENT_FORMAT] = {
      12,
      8,
      12,
      12
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

template <typename T>
      using ComPtr = Microsoft::WRL::ComPtr<T>;

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

struct VertexData {
      VertexLayout _layout;
      std::unique_ptr<char[]> _data;
      uint64_t vertexNum;
      uint64_t byteLength() {
            uint64_t len = 0;
            for (auto& l : _layout)
                  len += byte_Length[l];
            return len;
      }
};

VertexData make_example_vertexdata() {
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
      VertexData ret;
      ret._layout = VertexLayout({ POSITION3F32 });
      //char* vertex_data = new char[8 * ret.byteLength()];
      ret._data.reset(reinterpret_cast<char*>(vertex_data));
      ret.vertexNum = 8;

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