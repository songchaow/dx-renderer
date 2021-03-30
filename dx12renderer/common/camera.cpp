#include "common/camera.h"

Camera g_camera;

Matrix4 Frustum::cam2ndc_Perspective() const
{
      float tanHalfFov_H = std::tan(fov_Horizontal / 2);
      float tanHalfFov_V = tanHalfFov_H / aspectRatio;
      Matrix4 persp(1.f / tanHalfFov_H, 0, 0, 0,
            0, 1.f / tanHalfFov_V, 0, 0,
            0, 0, -(Far + Near) / (Far - Near), -2 * Far * Near / (Far - Near),
            0, 0, -1, 0);
      return persp;
}

Matrix4 Frustum::cam2ndc_Orthogonal() const
{
      // source x: [-width/2, width/2] |  source y: [-height/2, height/2]  output z:[-1, 1]
      // Far and near are absolute values here!
      Matrix4 orthogonal(2 / width, 0, 0, 0,
            0, 2 / height, 0, 0,
            0, 0, -2.f / (Far - Near), -(Far + Near) / (Far - Near),
            0, 0, 0, 1);
      return orthogonal;
}
