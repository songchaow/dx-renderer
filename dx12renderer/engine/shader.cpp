#include "shader.h"
#include <d3dcompiler.h>

std::string Shader::target_version = "_5_0";

ShaderPath _shaderPaths[(UINT)ShaderType::NUM_SHADER] = {
      {L"simple.hlsl",L"",L""},
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
