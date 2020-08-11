#pragma once
#include "engine/shader.h"

class RenderPass {
      Shader* shader;
      ID3D12RootSignature* root_signature; // owned
      VertexLayout input_layout;
      std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_storage;
      D3D12_RASTERIZER_DESC rasterizer_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
      D3D12_DEPTH_STENCIL_DESC depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
      D3D12_BLEND_DESC blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
      UINT sample_mask = UINT_MAX;
      D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

      // render targets


      D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;


      void fillShaderintoPSO();
      virtual void CreatePSO() {
            // set root signature
            pso_desc.pRootSignature = root_signature;
            // set input layout from input_layout
            CreateD3DInputLayoutDesc(input_layout, input_layout_storage, pso_desc.InputLayout);
            // set shader
            fillShaderintoPSO();
            
            // set raster, depth/stencil, blend state
            pso_desc.RasterizerState = rasterizer_state;
            pso_desc.DepthStencilState = depth_stencil_state;
            pso_desc.BlendState = blend_state;
            pso_desc.SampleMask = sample_mask;
            pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
            pso_desc.PrimitiveTopologyType = primitive_topology_type;

            // set num render targets
      }
};