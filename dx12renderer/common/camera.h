#pragma once
#include "geometry.h"

struct CameraState {
    Point3f pos;
    Vector3f viewDir;
    Vector3f upVec;

};

class Camera {
    CameraState posAndOrientation;

};