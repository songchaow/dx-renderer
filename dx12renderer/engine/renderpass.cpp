#include "engine/renderpass.h"
#include "d3dbootstrap.h"
#include "engine/scene.h"

D3D12_GRAPHICS_PIPELINE_STATE_DESC basic_pso_desc = {
      nullptr,
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      0,

};

void RenderPass::CreateRootSignature() {
      ComPtr<ID3DBlob> serializedRootSig = nullptr;
      ComPtr<ID3DBlob> errorBlob = nullptr;
      HRESULT hr = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1,
            serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

      if (errorBlob != nullptr)
      {
            ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
      }
      ThrowIfFailed(hr);
      ThrowIfFailed(g_pd3dDevice->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(root_signature.GetAddressOf())));
}

void RenderPass::draw()
{
      // switch pso
      g_pd3dCommandList->SetPipelineState(pso.Get());
      for (auto* p : Scene::scene.objs3D) {
            // set root descriptor table for const buffer (root param slot 1)
            g_pd3dCommandList->SetGraphicsRootDescriptorTable(1, p->ConstantBufferViewGPU());
      }

}

void CreateSimpleRenderPass() {
      // root signature desc
      CD3DX12_ROOT_PARAMETER root_param[2];

      // const buffer per frame
      CD3DX12_DESCRIPTOR_RANGE cbvTablePerFrame;
      cbvTablePerFrame.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
      // const buffer per object
      CD3DX12_DESCRIPTOR_RANGE cbvTablePerObject;
      cbvTablePerObject.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

      root_param[0].InitAsDescriptorTable(1, &cbvTablePerFrame);
      root_param[1].InitAsDescriptorTable(1, &cbvTablePerObject);

      // create root signature
      CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, root_param, 0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


}