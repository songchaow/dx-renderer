#pragma once
#include "engine/shader.h"
#include <string>
#include "common/geometry.h"

struct Resource {
      UINT id;
      enum ResourceType {
            TEXTURE2D,          // default heap
            RENDER_TARGET,    // can also be texture, default heap
            CONST_BUFFER,     // upload heap
      };
      ResourceType type;
      //union ResourceSize {
      //      Point2i size2d; // element counts, NOT byte size
      //      uint32_t length; // byte length?
      //};
      //ResourceSize size;
      uint32_t depth = 1;
      D3D12_RESOURCE_DESC desc;
      ComPtr<ID3D12Resource> resource;
      D3D12_RESOURCE_STATES curr_state;
      void Create();

      // render target or texture2d
      Resource(ResourceType type, UINT width, UINT height, DXGI_FORMAT element_format, UINT depth = 1, UINT16 miplevels = 1,
            D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_GENERIC_READ, UINT sampleCount = 1, UINT sampleQuality = 0)
            : type(type), curr_state(initialState) {
            desc.Alignment = 0;
            desc.MipLevels = miplevels;
            desc.Format = element_format;
            desc.SampleDesc.Count = sampleCount;
            desc.SampleDesc.Quality = sampleQuality;
            desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            
            desc.Width = width; desc.Height = height; desc.DepthOrArraySize = depth;
            if (type == RENDER_TARGET)
                  desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

      }


      
};
#include <cassert>

class RenderPass {
      std::string name;
      Shader* shader;
      VertexLayout input_layout;
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
      std::vector<Resource*> render_targets;
      Resource* depth_stencil = nullptr;


      void fillShaderintoPSO() {
            if (!shader->isCompiled()) {
                  shader->compileAndLink();
                  shader->setVSByteCode(pso_desc.VS);
                  shader->setGSByteCode(pso_desc.GS);
                  shader->setPSByteCode(pso_desc.PS);
            }
      }


      void fillShaderintoPSODesc() {
            assert(shader!=nullptr);
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
      virtual void CreatePSO() {
            // set root signature
            pso_desc.pRootSignature = root_signature.Get();
            // set input layout from input_layout
            CreateD3DInputLayoutDesc(input_layout, input_layout_storage, pso_desc.InputLayout);
            // set shader
            fillShaderintoPSODesc();
            
            // set raster, depth/stencil, blend state
            pso_desc.RasterizerState = rasterizer_state;
            pso_desc.DepthStencilState = depth_stencil_state;
            pso_desc.BlendState = blend_state;
            pso_desc.SampleMask = sample_mask;
            pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
            pso_desc.PrimitiveTopologyType = primitive_topology_type;

            // set num render targets
            pso_desc.NumRenderTargets = render_targets.size();
            for (uint32_t i = 0; i < render_targets.size(); i++) {
                  pso_desc.RTVFormats[i] = render_targets[i]->desc.Format;
            }
            if (depth_stencil)
                  pso_desc.DSVFormat = depth_stencil->desc.Format;
            else
                  pso_desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
      }
      virtual void draw();

public:
      RenderPass(std::string name, Shader* s, VertexLayout vl, D3D12_ROOT_SIGNATURE_DESC rootsig_desc) : name(name), shader(s),
            input_layout(vl), root_signature_desc(rootsig_desc) {}
};