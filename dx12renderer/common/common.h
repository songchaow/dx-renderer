#pragma once
#include "stdafx.h"
#include <d3d12.h>
#include <dxgi1_4.h>

enum class CBVLocation {
      IMGUI = 0,
      PER_FRAME = 1,
      PER_OBJECT = 2,
      NUM_CBV
};

constexpr int NUM_FRAMES_IN_FLIGHT = 3;
constexpr int NUM_BACK_BUFFERS = 3;
extern ID3D12Device* g_pd3dDevice;
extern ID3D12DescriptorHeap* g_pd3dSrvDescHeap;
extern ID3D12DescriptorHeap*        g_pd3dRtvDescHeap;
extern uint32_t                     g_nextRtvDescIdx;
extern IDXGISwapChain3*             g_pSwapChain;
extern ID3D12CommandQueue*          g_pd3dCommandQueue;
extern UINT64                       g_fenceLastSignaledValue; // global
extern ID3D12Fence*                 g_fence;
extern ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS];
extern D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS];
extern ID3D12GraphicsCommandList*   g_pd3dCommandList;
extern UINT                         g_frameIndex;
extern HANDLE                       g_hSwapChainWaitableObject;

constexpr int NUM_RESERVED_CBV_SRV_UAV = 1024;
constexpr int NUM_RESERVED_RTV = 10 * NUM_BACK_BUFFERS;