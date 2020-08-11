#include "primitive.h"

uint32_t Primitive3D::curr_max_pIdx = 0;
UploadBuffer<Primitive3D::DataPerObject> Primitive3D::constant_buffer;