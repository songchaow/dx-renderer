#include "stdafx.h"
#include "dx12renderer.h"
#include "d3dbootstrap.h"
#include "d3dx12.h"

#include "engine/primitive.h"

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif


FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
UINT                         g_frameIndex = 0;

ID3D12Device*                g_pd3dDevice = NULL;
static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL;
ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;
ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL;
ID3D12Fence*                 g_fence = NULL;
static HANDLE                       g_fenceEvent = NULL;
UINT64                       g_fenceLastSignaledValue = 0;
IDXGISwapChain3*             g_pSwapChain = NULL;
static HANDLE                       g_hSwapChainWaitableObject = NULL;
ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
ID3D12RootSignature* g_defaultRootSignature = NULL;

void CreateRTVfromSwapChain()
{
      for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
      {
            ID3D12Resource* pBackBuffer = NULL;
            g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
            g_mainRenderTargetResource[i] = pBackBuffer;
      }
}
void WaitForLastSubmittedFrame()
{
      FrameContext* frameCtxt = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

      UINT64 fenceValue = frameCtxt->FenceValue;
      if (fenceValue == 0)
            return; // No fence was signaled

      frameCtxt->FenceValue = 0;
      if (g_fence->GetCompletedValue() >= fenceValue)
            return;

      g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
      WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtxt = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtxt->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtxt->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtxt;
}

void CleanupRenderTarget()
{
      WaitForLastSubmittedFrame();

      for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
            if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

bool CreatePipelineD3D() {
      

      // bind to pipeline
      //g_pd3dCommandList->SetGraphicsRootSignature(g_defaultRootSignature); // set from render pass when used for the first time
      g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);

      // future: read from a list of RenderPass, and create necessary resources
      // now, just create manually


#if 0
      CD3DX12_GPU_DESCRIPTOR_HANDLE cbv(g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
      SIZE_T size = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
      cbv.Offset(1, size);
      g_pd3dCommandList->SetGraphicsRootDescriptorTable(0, cbv);
#endif

      return false;
}

bool CreateDeviceD3D(HWND hWnd)
{
#ifdef DX12_ENABLE_DEBUG_LAYER
      ID3D12Debug* pdx12Debug = NULL;
      if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
      {
            pdx12Debug->EnableDebugLayer();
            pdx12Debug->Release();
      }
#endif

      D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
      if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
            return false;

      {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = NUM_BACK_BUFFERS;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 1;
            if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
                  return false;

            SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
            for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
            {
                  g_mainRenderTargetDescriptor[i] = rtvHandle;
                  rtvHandle.ptr += rtvDescriptorSize;
            }
      }

      {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            // the first for imgui, the second for per frame, and others for constant buffer per object
            desc.NumDescriptors = UINT(CBVLocation::NUM_CBV); 
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
                  return false;
      }

      {
            D3D12_COMMAND_QUEUE_DESC desc = {};
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.NodeMask = 1;
            if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
                  return false;
      }

      for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
            if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
                  return false;

      if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
            g_pd3dCommandList->Close() != S_OK)
            return false;

      if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
            return false;

      g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
      if (g_fenceEvent == NULL)
            return false;

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
                  return false;
            swapChain1->Release();
            dxgiFactory->Release();
            g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
            g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
      }

      CreateRTVfromSwapChain();
      return true;
}

void CleanupDeviceD3D()
{
      CleanupRenderTarget();
      if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
      if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
      for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
            if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
      if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
      if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
      if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
      if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
      if (g_fence) { g_fence->Release(); g_fence = NULL; }
      if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
      if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
      if (g_defaultRootSignature) { g_defaultRootSignature->Release(); g_defaultRootSignature = NULL; }

#ifdef DX12_ENABLE_DEBUG_LAYER
      IDXGIDebug1* pDebug = NULL;
      if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
      {
            pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
            pDebug->Release();
      }
#endif
}