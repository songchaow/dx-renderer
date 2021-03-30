#include "ext/imgui/imgui.h"
#include "ext/imgui/imgui_impl_win32.h"
#include "ext/imgui/imgui_impl_dx12.h"

#include "engine/scene.h"
#include "engine/shader.h"
#include "d3dbootstrap.h"
#include "main.h"

void RenderUI() {
      static bool show_demo_window = true;
      static bool show_another_window = false;
      const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

      // Start the Dear ImGui frame
      ImGui_ImplDX12_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();

      // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
      if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
      {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                  counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
      }

      // 3. Show another simple window.
      if (show_another_window)
      {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                  show_another_window = false;
            ImGui::End();
      }
}

void RenderFrame() {
      FrameContext* frameCtxt = WaitForNextFrameResources();
      UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
      frameCtxt->CommandAllocator->Reset();

      // TODO: update camera

      // TODO: render scene here

      RenderUI();


      D3D12_RESOURCE_BARRIER barrier = {};
      barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
      barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
      barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
      barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
      barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
      barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

      g_pd3dCommandList->Reset(frameCtxt->CommandAllocator, NULL);
      g_pd3dCommandList->ResourceBarrier(1, &barrier);
      ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
      g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], (float*)&clear_color, 0, NULL);
      g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
      g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);

      ImGui::Render();
      ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
      barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
      barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
      g_pd3dCommandList->ResourceBarrier(1, &barrier);

      g_pd3dCommandList->Close();

      g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

      g_pSwapChain->Present(0, 0);

      UINT64 fenceValue = g_fenceLastSignaledValue + 1;
      g_pd3dCommandQueue->Signal(g_fence, fenceValue);
      g_fenceLastSignaledValue = fenceValue;
      frameCtxt->FenceValue = fenceValue;



}

bool d3dbootstrap(HWND hwindow) {
      // Initialize D3D
      if (!CreateDeviceD3D(hwindow))
      {
            CleanupDeviceD3D();
            ::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
            return false;
      }


      // begin record initialization commands
      g_pd3dCommandList->Reset(g_frameContext[0].CommandAllocator, nullptr);

      // constant buffer of primitives (no commands)
      Primitive3D::initConstBuffer();

      // Shader (no commands)
      ShaderStore::shaderStore.init();


      // Pipeline
      CreatePipelineD3D();

      // Load primitives (loading contains commands)
      Scene::scene.objs3D.push_back(make_example_primitive());

      // execute and initialize
      g_pd3dCommandList->Close();
      ID3D12CommandList* cmdLists = { g_pd3dCommandList };
      g_pd3dCommandQueue->ExecuteCommandLists(1, &cmdLists);

      // Setup Dear ImGui context
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO(); (void)io;

      ImGui::StyleColorsDark();

      // Setup Platform/Renderer bindings

      ImGui_ImplWin32_Init(hwindow);
      ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
            DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
            g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
            g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

}