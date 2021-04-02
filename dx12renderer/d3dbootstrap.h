#pragma once
#include "stdafx.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include "utility/UploadBuffer.h"
#include "common/transform.h"

struct DataPerPrimitive3D {
      union {
            DirectX::XMFLOAT4X4 obj2world;
            Matrix4 _obj2world;
      };
      DataPerPrimitive3D() : _obj2world() {}
};

struct DataPerPass {
      union {
            DirectX::XMFLOAT4X4 world2cam;
            Matrix4 _world2cam;
      };
      union {
            DirectX::XMFLOAT4X4 cam2ndc;
            Matrix4 _cam2ndc;
      };
      DataPerPass() : _world2cam(), _cam2ndc() {}
};

struct FrameContext
{
      ID3D12CommandAllocator* CommandAllocator;
      // Each frame needs their own const buffers!
      //UploadBuffer<DataPerPass> cbuffer_per_pass; // Now we use CBuffer!
      UploadBuffer<DataPerPrimitive3D> constant_buffer_all_primitive3d;
      UINT64                  FenceValue;
};

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
extern FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT];
extern IDXGISwapChain3*             g_pSwapChain;
extern ID3D12CommandQueue*          g_pd3dCommandQueue;
extern UINT64                       g_fenceLastSignaledValue; // global
extern ID3D12Fence*                 g_fence;
extern ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS];
extern D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS];
extern ID3D12GraphicsCommandList*   g_pd3dCommandList;
extern UINT                         g_frameIndex;

constexpr int NUM_RESERVED_CBV_SRV_UAV = 1024;
constexpr int NUM_RESERVED_RTV = 10 * NUM_BACK_BUFFERS;

void CreateRTVfromSwapChain();
void WaitForLastSubmittedFrame();
void CleanupRenderTarget();
bool CreateDeviceD3D(HWND hWnd);
bool CreatePipelineD3D();
void CleanupDeviceD3D();

FrameContext* WaitForNextFrameResources();