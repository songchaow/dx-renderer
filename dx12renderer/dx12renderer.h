#pragma once

#include "resource.h"

#include "engine/scene.h"
#include "engine/renderpass.h"
#include "utility/UploadBuffer.h"


class D3DRenderer {

      Scene _scene;
      D3DTexture _swapBuffer;
      std::vector<RenderPass> renderpass_graph;

public:
      bool d3d_init(HWND hwindow);
      void render_frame();

};

void RenderUI();