#pragma once
#include "d3dx12.h"
#include "stdafx.h"
#include "wrl/client.h"

template <typename T>
      using ComPtr = Microsoft::WRL::ComPtr<T>;

class Primitive3D {
      // constant buffer content
      struct DataPerObject {
            DirectX::XMFLOAT4X4 obj2world;
      };
      ComPtr<ID3D12Resource> constant_buffer = nullptr;
      void initD3D() {

      }
};