#include "utility/UploadBuffer.h"
#include "d3dbootstrap.h"

void Resource::transit_if_needed(D3D12_RESOURCE_STATES target_state) {
      if (curr_state != target_state) {
            g_pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
                  curr_state, target_state));
            curr_state = target_state;
      }
}
