# DX-Renderer

## To-dos

### Resource related
- Unify ways of managing resources
- Use `Resource` struct in `UploadBuffer`, vertex in `Primitive3D`

- Add a method `CreateAsRenderTarget`, input arguments are `Resource`?
- Clear render targets and depth/stencil buffers
- Set render target! in each RenderPass::draw() `OMSetRenderTargets`
  - we need CPU handles. how to get cpu handle from Resource? Seems we need heapstart() (the render target heap) and then the offset?

### Material related