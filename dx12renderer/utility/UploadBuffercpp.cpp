#include "utility/UploadBuffer.h"
#include "d3dbootstrap.h"

void Resource::transit_if_needed(D3D12_RESOURCE_STATES target_state) {
      if (curr_state != target_state) {
            g_pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
                  curr_state, target_state));
            curr_state = target_state;
      }
}

void Resource::CreateAsConstBuffer(uint32_t buffer_size) {
      type = CONST_BUFFER;
      upload_heap = true;

      // for cbuffer, adjust the size
      buffer_size = d3dUtil::CalcConstantBufferByteSize(buffer_size);

      desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags);
      ThrowIfFailed(g_pd3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
            curr_state,
            nullptr,
            IID_PPV_ARGS(resource.GetAddressOf())));
      ThrowIfFailed(resource->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
}

void Resource::CreateAsVertexIdxBuffer(uint32_t buffer_size) {
      //on default heap, but shouldn't care the exact type here
      type = VERTEX_IDX_DATA;
      upload_heap = false;
      desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags);
      ThrowIfFailed(g_pd3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
            curr_state,
            nullptr,
            IID_PPV_ARGS(resource.GetAddressOf())));
}

void Resource::_upload_to_default_heap(void* data, uint32_t data_byte_size, uint32_t offset) {
      // create intermediate default buffer
      ThrowIfFailed(g_pd3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(data_byte_size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(_upload_buffer.GetAddressOf())));
      // Describe the data we want to copy into the default buffer.
      D3D12_SUBRESOURCE_DATA subResourceData = {};
      subResourceData.pData = data;
      subResourceData.RowPitch = data_byte_size;
      subResourceData.SlicePitch = subResourceData.RowPitch;

      // Upload
      g_pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
      // The meaning of offset in this helper function:
      // 1. copy the [offset : offset+bytelength] part from subresource to the beginning of the upload buffer
      // 2. copy the entire part of the upload buffer to the [offset : offset+bytelength] region of the default buffer
      UpdateSubresources<1>(g_pd3dCommandList, resource.Get(), _upload_buffer.Get(), offset, 0, 1, &subResourceData);
      g_pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
      // TODO: free the upload buffer when we are sure the copy is completed.
}