#pragma once
#include "d3dx12.h"
#include "utility/utility.h"
#include "engine/VetexFormat.h"

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

struct ShaderConfig {
      ShaderPath path;
      VertexLayoutDesc input_layout;

};

extern ShaderPath _shaderPaths[(UINT)ShaderType::NUM_SHADER];

class ShaderStore;

class Shader {
protected:
      bool _compile = false;
      D3D12_INPUT_LAYOUT_DESC input_layout_desc;
      std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_storage;
      ShaderPath path;
      std::vector<D3D_SHADER_MACRO> defines;
      struct ShaderBinary {
            ComPtr<ID3DBlob> binary_vs;
            ComPtr<ID3DBlob> binary_gs;
            ComPtr<ID3DBlob> binary_ps;
      };
      ShaderBinary _binary;
      ComPtr<ID3DBlob> err_msg;
      static std::string target_version;
public:
      bool compileAndLink();
      Shader() = default;
      Shader(ShaderPath path) : path(path) {}
      bool isCompiled() const { return _compile; }
      friend class ShaderStore;
      void setVSByteCode(D3D12_SHADER_BYTECODE& bytecode);
      void setGSByteCode(D3D12_SHADER_BYTECODE& bytecode);
      void setPSByteCode(D3D12_SHADER_BYTECODE& bytecode);
      const ShaderBinary GetBinary() const { return _binary; }
};

class ShaderStore {
      Shader store[(UINT)ShaderType::NUM_SHADER];
public:
      static ShaderStore shaderStore;
      ShaderStore();
      void init();
      Shader& operator[](int32_t idx) { return store[idx]; }
};