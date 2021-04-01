#include "engine/renderpass.h"
#include "d3dbootstrap.h"
#include "engine/scene.h"
#include "common/camera.h"

D3D12_GRAPHICS_PIPELINE_STATE_DESC basic_pso_desc = {
      nullptr,
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      D3D12_SHADER_BYTECODE(),
      0,

};

void RenderPass::build_cbuffer_data_perpass() {
      DataPerPass val;
      val._world2cam = g_camera.world2cam();
      val._cam2ndc = g_camera.cam2ndc();
      cbuffer.update_cpu_buffer_single(&val);
      cbuffer.updateGPUAll(0);
}

void RenderPass::update_cbuffer_data_perpass() {
      if (g_camera.world2cam_dirty()) {
            DataPerPass val;
            val._world2cam = g_camera.world2cam();
            val._cam2ndc = g_camera.cam2ndc();
            cbuffer.update_cpu_buffer_single(&val);
            cbuffer.updateGPUCurrFrame(0);
      }
}

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

void RenderPass::draw(std::vector<Resource>& rts)
{
      // switch pso
      g_pd3dCommandList->SetPipelineState(pso.Get());
      // update const buffer if changed
      update_cbuffer_data_perpass();
      g_pd3dCommandList->SetGraphicsRootConstantBufferView(0, cbuffer.gpu_addr_curr_frame(0));
      // set input Resource states (vertex? textures?)

      // set output Resource states (render targets)
      for(auto& rt : rts) {
            rt.transit_if_needed(D3D12_RESOURCE_STATE_RENDER_TARGET);
      }

      // render each item
      // TODO: set render target

      // TODO: resource transition on render target, input sources
      for (auto* p : Scene::scene.objs3D) {
            // Input
            g_pd3dCommandList->IASetVertexBuffers(0, 1, p->VertexBufferView());
            g_pd3dCommandList->IASetIndexBuffer(p->IndexBufferView());
            g_pd3dCommandList->IASetPrimitiveTopology(p->Topology());
            // TODO: update the following from the outside
            //g_pd3dCommandList->SetGraphicsRootConstantBufferView(0, );
            g_pd3dCommandList->SetGraphicsRootConstantBufferView(1, p->ConstBufferGPUAddressCurrFrame());

            // TODO: if dirty, and the count >0, reduce the count, and update the cb of current frame.
            p->update_gpu_cbuffer();

            g_pd3dCommandList->DrawIndexedInstanced(p->mesh().indexNum, 1, 0, 0, 0);
      }

}

void RenderPass::CreatePSO() {
      // RootSignature
      pso_desc.pRootSignature = root_signature.Get();
      // InputLayout
      CreateD3DInputLayoutDesc(input_layout, input_layout_storage, pso_desc.InputLayout);
      // Shader
      fillShaderintoPSODesc();

      // set raster, depth/stencil, blend state, sample count
      pso_desc.RasterizerState = rasterizer_state;
      pso_desc.DepthStencilState = depth_stencil_state;
      pso_desc.BlendState = blend_state;
      pso_desc.SampleMask = sample_mask;
      pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
      pso_desc.PrimitiveTopologyType = primitive_topology_type;
      pso_desc.SampleDesc.Count = 1;
      pso_desc.SampleDesc.Quality = 0;


      // set num render targets
      if (render_targets.size() > 0) {
            pso_desc.NumRenderTargets = rt_formats.size();
            for (uint32_t i = 0; i < rt_formats.size(); i++) {
                  pso_desc.RTVFormats[i] = rt_formats[i];
            }
      }
      else {
            pso_desc.NumRenderTargets = render_targets.size();
            for (uint32_t i = 0; i < render_targets.size(); i++) {
                  pso_desc.RTVFormats[i] = render_targets[i]->desc.Format;
            }
      }

      if (depth_stencil)
            pso_desc.DSVFormat = depth_stencil->desc.Format;
      else
            pso_desc.DSVFormat = ds_format;

      ThrowIfFailed(g_pd3dDevice->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(pso.GetAddressOf())));
}

RenderPass CreateSimpleRenderPass() {
      // root signature desc
      CD3DX12_ROOT_PARAMETER root_param[2];
      /*
      // const buffer per frame
      CD3DX12_DESCRIPTOR_RANGE cbvTablePerFrame;
      cbvTablePerFrame.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
      */

      // const buffer per object
      //CD3DX12_DESCRIPTOR_RANGE cbvTablePerObject;
      //cbvTablePerObject.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

      //root_param[1].InitAsDescriptorTable(1, &cbvTablePerObject);
      root_param[0].InitAsConstantBufferView(0); // per frame
      root_param[1].InitAsConstantBufferView(1); // per object
      //root_param[2].InitAsDescriptorTable(1, &cbvTablePerFrame);

      // create root signature
      CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, root_param, 0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
      // rt formats
      std::vector<DXGI_FORMAT> rt_formats = { DXGI_FORMAT_R8G8B8A8_UINT };
      RenderPass simplePass("SimplePass", &ShaderStore::shaderStore[int32_t(ShaderType::TEST_SHADER)],
            { POSITION3F32 }, rootSigDesc, rt_formats, DXGI_FORMAT_R24G8_TYPELESS);
      return simplePass;
}

void Resource::Create()
{
      ThrowIfFailed(g_pd3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
            &desc, curr_state, nullptr, IID_PPV_ARGS(resource.GetAddressOf())));
}
