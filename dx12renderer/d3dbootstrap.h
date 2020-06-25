#pragma once
#include "stdafx.h"
#include <d3d12.h>
#include <dxgi1_4.h>

extern ID3D12Device* g_pd3dDevice;
extern ID3D12DescriptorHeap* g_pd3dSrvDescHeap;
constexpr int NUM_FRAMES_IN_FLIGHT = 3;

struct FrameContext
{
      ID3D12CommandAllocator* CommandAllocator;
      UINT64                  FenceValue;
};

void CreateRenderTarget();
void WaitForLastSubmittedFrame();
void CleanupRenderTarget();
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();