#pragma once
#include "d3dx12.h"
#include "utility/utility.h"

enum class ShaderType {
      TEST_SHADER,
      NUM_SHADER
};

struct ShaderPath {
      std::wstring vertex;
      std::wstring geometry;
      std::wstring fragment;
      bool complete() const { return vertex.size() > 0; }
};

class Shader {
      ShaderPath path;
      std::vector<D3D_SHADER_MACRO> defines;
      ComPtr<ID3DBlob> binary_vs;
      ComPtr<ID3DBlob> binary_gs;
      ComPtr<ID3DBlob> binary_ps;
      ComPtr<ID3DBlob> err_msg;
      static std::string target_version;
      void compileAndLink();
};