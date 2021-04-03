#include "utility/UploadBuffer.h"
#include "d3dbootstrap.h"

void Resource::transit_if_needed(D3D12_RESOURCE_STATES target_state) {
      if (curr_state != target_state) {
            g_pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
                  curr_state, target_state));
            curr_state = target_state;
      }
}

void Resource::CreateAsConstBuffer(uint32_t buffer_size) {
      type = CONST_BUFFER;
      upload_heap = true;

      // for cbuffer, adjust the size
      buffer_size = d3dUtil::CalcConstantBufferByteSize(buffer_size);

      D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags);
      ThrowIfFailed(g_pd3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
            curr_state,
            nullptr,
            IID_PPV_ARGS(resource.GetAddressOf())));
      ThrowIfFailed(resource->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
}

void Resource::CreateAsVertexIdxBuffer(uint32_t buffer_size) {
      //on default heap, but shouldn't care the exact type here
      type = VERTEX_IDX_DATA;
      upload_heap = false;
      D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags);
      ThrowIfFailed(g_pd3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
            curr_state,
            nullptr,
            IID_PPV_ARGS(resource.GetAddressOf())));
}

void Resource::CreateAs2DTexture(DXGI_FORMAT element_format)
{

}

void Resource::CreateFromExistingResource(ID3D12Resource * created, ResourceType type_, D3D12_RESOURCE_STATES init_state)
{
      type = type_;
      upload_heap = false;
      curr_state = init_state;
      resource = created;
      created->GetDesc();
}

void Resource::_upload_to_default_heap(void* data, uint32_t data_byte_size, uint32_t offset) {
      // create intermediate default buffer
      ThrowIfFailed(g_pd3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(data_byte_size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(_upload_buffer.GetAddressOf())));
      // Describe the data we want to copy into the default buffer.
      D3D12_SUBRESOURCE_DATA subResourceData = {};
      subResourceData.pData = data;
      subResourceData.RowPitch = data_byte_size;
      subResourceData.SlicePitch = subResourceData.RowPitch;

      // Upload
      g_pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
      // The meaning of offset in this helper function:
      // 1. copy the [offset : offset+bytelength] part from subresource to the beginning of the upload buffer
      // 2. copy the entire part of the upload buffer to the [offset : offset+bytelength] region of the default buffer
      UpdateSubresources<1>(g_pd3dCommandList, resource.Get(), _upload_buffer.Get(), offset, 0, 1, &subResourceData);
      g_pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
      // TODO: free the upload buffer when we are sure the copy is completed.
}

void D3DTexture::CreateAsSwapChain(HWND hWnd, DXGI_FORMAT element_format)
{
      perframe = true;
      enable_render_target = true;
      {
            // Setup swap chain
            DXGI_SWAP_CHAIN_DESC1 sd;
            {
                  ZeroMemory(&sd, sizeof(sd));
                  sd.BufferCount = NUM_BACK_BUFFERS;
                  sd.Width = 0;
                  sd.Height = 0;
                  sd.Format = element_format;
                  sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
                  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                  sd.SampleDesc.Count = 1;
                  sd.SampleDesc.Quality = 0;
                  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                  sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
                  sd.Scaling = DXGI_SCALING_STRETCH;
                  sd.Stereo = FALSE;
            }
            IDXGIFactory4* dxgiFactory = NULL;
            IDXGISwapChain1* swapChain1 = NULL;
            if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK ||
                  dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK ||
                  swapChain1->QueryInterface(IID_PPV_ARGS(&_swapChain)) != S_OK)
                  return; // TODO: log error
            swapChain1->Release();
            dxgiFactory->Release();
            _swapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
            g_hSwapChainWaitableObject = _swapChain->GetFrameLatencyWaitableObject();
            // also set the global swapchain object for compatibility
            g_pSwapChain = _swapChain;
      }

      // set cpu descriptor
      _cpu_handle_start_rtv = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
      SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
      _cpu_handle_start_rtv.ptr += g_nextRtvDescIdx * rtvDescriptorSize;

      g_nextRtvDescIdx += NUM_BACK_BUFFERS;

      // Create rtvs
      D3D12_CPU_DESCRIPTOR_HANDLE local_hdl = _cpu_handle_start_rtv;
      for (int i = 0; i < NUM_BACK_BUFFERS; i++) {
            ID3D12Resource* curr_buffer = nullptr;
            _swapChain->GetBuffer(i, IID_PPV_ARGS(&curr_buffer));
            g_pd3dDevice->CreateRenderTargetView(curr_buffer, NULL, local_hdl);
            _resource[i].CreateFromExistingResource(curr_buffer, Resource::ResourceType::SWAP_CHAIN);
            local_hdl.ptr += rtvDescriptorSize;
      }

      // TODO: (optional) also create srv?
}
