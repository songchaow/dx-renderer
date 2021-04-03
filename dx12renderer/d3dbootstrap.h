#pragma once
#include "stdafx.h"
#include "common/common.h"
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

extern FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT];



void fillResoourcePtrfromSwapChain();
void WaitForLastSubmittedFrame();
void CleanupRenderTarget();
bool CreateDeviceD3D(HWND hWnd);
bool CreatePipelineD3D();
void CleanupDeviceD3D();
FrameContext* WaitForNextFrameResources();