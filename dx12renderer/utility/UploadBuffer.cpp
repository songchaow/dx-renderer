#include "UploadBuffer.h"
#include "d3dbootstrap.h"

void D3DTexture::CreateAsSwapChain(HWND hWnd)
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
                  sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
                  swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
                  return; // TODO: log error
            swapChain1->Release();
            dxgiFactory->Release();
            g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
            g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
      }

      // TODO: isrendertarget is true, append a descriptor in RTV heap
      _cpu_handle_start = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
      SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
      _cpu_handle_start.ptr += g_nextRtvDescIdx * rtvDescriptorSize;

      // Create rtvs
      D3D12_CPU_DESCRIPTOR_HANDLE local_hdl = _cpu_handle_start;
      for (int i = 0; i < NUM_BACK_BUFFERS; i++) {
            ID3D12Resource* curr_buffer = nullptr;
            g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&curr_buffer));
            g_pd3dDevice->CreateRenderTargetView(curr_buffer, NULL, local_hdl);
            local_hdl.ptr += rtvDescriptorSize;
      }

}
