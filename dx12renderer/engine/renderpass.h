#pragma once
#include "engine/shader.h"
#include <string>
#include "common/geometry.h"
#include "utility/CBuffer.h"
#include "d3dbootstrap.h"
#include <cassert>

class RenderPass {
      std::string name;
      Shader* shader;
      VertexLayoutDesc input_layout;
      D3D12_ROOT_SIGNATURE_DESC root_signature_desc;
      ComPtr< ID3D12RootSignature> root_signature;
      //ID3D12RootSignature* root_signature; // owned

      // other attributes
      D3D12_RASTERIZER_DESC rasterizer_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
      D3D12_DEPTH_STENCIL_DESC depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
      D3D12_BLEND_DESC blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
      UINT sample_mask = UINT_MAX;
      D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
      
      //uint32_t num_render_targets = 1;
      

      std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_storage;
      D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;
      ComPtr<ID3D12PipelineState> pso;
      // render targets
      std::vector<DXGI_FORMAT> rt_formats;
      DXGI_FORMAT ds_format = DXGI_FORMAT_UNKNOWN;
      std::vector<Resource*> render_targets; // not necessarily valid
      Resource* depth_stencil = nullptr;

      // cbuffer
      CBuffer<DataPerPass> cbuffer;

      void fillShaderintoPSODesc() {
            assert(shader!=nullptr);
            if (!shader->isCompiled())
                  shader->compileAndLink();
            auto bin = shader->GetBinary();
            pso_desc.VS = {
                  bin.binary_vs->GetBufferPointer(),
                  bin.binary_vs->GetBufferSize()
            };
            pso_desc.GS = {
                  bin.binary_gs->GetBufferPointer(),
                  bin.binary_gs->GetBufferSize()
            };
            pso_desc.PS = {
                  bin.binary_ps->GetBufferPointer(),
                  bin.binary_ps->GetBufferSize()
            };
      }
      void CreateRootSignature();
      virtual void CreatePSO();
      // Called when initializing
      // Builds cbuffer data content, stores in host memory and uploads to GPU cbuffer
      void build_cbuffer_data_perpass();
      // Called per frame
      void update_cbuffer_data_perpass();
      void init() {
            build_cbuffer_data_perpass();
            CreateRootSignature();
            CreatePSO();
      }
      virtual void draw(std::vector<Resource*>& rt);

public:
      // d3dDevice must be valid
      RenderPass(std::string name, Shader* s, VertexLayoutDesc vl, D3D12_ROOT_SIGNATURE_DESC rootsig_desc,
            std::vector<DXGI_FORMAT> rt_formats, DXGI_FORMAT ds_format) : name(name), shader(s), input_layout(vl),
             root_signature_desc(rootsig_desc), rt_formats(rt_formats), ds_format(ds_format), cbuffer(g_pd3dDevice) {}
};

RenderPass CreateSimpleRenderPass();