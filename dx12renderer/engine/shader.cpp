#include "shader.h"
#include <d3dcompiler.h>

std::string Shader::target_version = "_5_0";

ShaderPath _shaderPaths[(UINT)ShaderType::NUM_SHADER] = {
      {L"simple.hlsl",L"",L""},
};

ShaderConfig _shaderConfigs[(UINT)ShaderType::NUM_SHADER] = {
      {{L"simple.hlsl",L"",L""}, {POSITION3F32}}
};

void Shader::compileAndLink()
{
      UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
      compileFlags = D3DCOMPILE_DEBUG |
            D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
      if (path.vertex.size() > 0) {
            HRESULT h = D3DCompileFromFile(path.vertex.c_str(), defines.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", ("vs" + target_version).c_str(),
                  compileFlags, 0, binary_vs.GetAddressOf(), err_msg.GetAddressOf());
            if (err_msg != nullptr)
                  OutputDebugStringA((char*)err_msg->GetBufferPointer());
            ThrowIfFailed(h);
      }
      if (path.geometry.size() > 0) {
            HRESULT h = D3DCompileFromFile(path.vertex.c_str(), defines.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", ("gs" + target_version).c_str(),
                  compileFlags, 0, binary_gs.GetAddressOf(), err_msg.GetAddressOf());
            if (err_msg != nullptr)
                  OutputDebugStringA((char*)err_msg->GetBufferPointer());
            ThrowIfFailed(h);
      }
      if (path.fragment.size() > 0) {
            HRESULT h = D3DCompileFromFile(path.vertex.c_str(), defines.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", ("ps" + target_version).c_str(),
                  compileFlags, 0, binary_ps.GetAddressOf(), err_msg.GetAddressOf());
            if (err_msg != nullptr)
                  OutputDebugStringA((char*)err_msg->GetBufferPointer());
            ThrowIfFailed(h);
      }
}

ShaderStore::ShaderStore() {
      for (uint32_t i = 0; i < (UINT)ShaderType::NUM_SHADER; i++) {
            store[i] = Shader(_shaderPaths[i]);
            CreateD3DInputLayoutDesc(_shaderConfigs[i].input_layout, store[i].input_layout_storage, store[i].input_layout_desc);
      }
}

void ShaderStore::init()
{
      for (auto& s : store)
            s.compileAndLink();
}
