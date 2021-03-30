#include "common/camera.h"

Matrix4 Frustum::cam2ndc_Perspective() const
{
      float tanHalfFov_H = std::tan(fov_Horizontal / 2);
      float tanHalfFov_V = tanHalfFov_H / aspectRatio;
      Matrix4 persp(1.f / tanHalfFov_H, 0, 0, 0,
            0, 1.f / tanHalfFov_V, 0, 0,
            0, 0, -(Far + near) / (Far - near), -2 * Far * near / (Far - near),
            0, 0, -1, 0);
      return persp;
}

Matrix4 Frustum::cam2ndc_Orthogonal() const
{
      // source x: [-width/2, width/2] |  source y: [-height/2, height/2]  output z:[-1, 1]
      // Far and near are absolute values here!
      Matrix4 orthogonal(2 / width, 0, 0, 0,
            0, 2 / height, 0, 0,
            0, 0, -2.f / (Far - near), -(Far + near) / (Far - near),
            0, 0, 0, 1);
      return orthogonal;
}
