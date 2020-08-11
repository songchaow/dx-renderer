#pragma once
#include "stdafx.h"
#include <d3d12.h>
#include <dxgi1_4.h>

struct FrameContext
{
      ID3D12CommandAllocator* CommandAllocator;
      UINT64                  FenceValue;
};

constexpr int NUM_FRAMES_IN_FLIGHT = 3;
constexpr int NUM_BACK_BUFFERS = 3;
extern ID3D12Device* g_pd3dDevice;
extern ID3D12DescriptorHeap* g_pd3dSrvDescHeap;
extern FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT];
extern IDXGISwapChain3*             g_pSwapChain;
extern ID3D12CommandQueue*          g_pd3dCommandQueue;
extern UINT64                       g_fenceLastSignaledValue; // global
extern ID3D12Fence*                 g_fence;
extern ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS];
extern D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS];
extern ID3D12GraphicsCommandList*   g_pd3dCommandList;



void CreateRenderTarget();
void WaitForLastSubmittedFrame();
void CleanupRenderTarget();
bool CreateDeviceD3D(HWND hWnd);
bool CreatePipelineD3D();
void CleanupDeviceD3D();

FrameContext* WaitForNextFrameResources();