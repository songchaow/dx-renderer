#pragma once
#include "geometry.h"
#include "common/transform.h"

struct CameraState {
    Point3f pos;
    Vector3f viewDir;
    Vector3f upVec;

    void renormalize(Vector3f& Xcam, Vector3f& Ycam, Vector3f& Zcam) {
          Zcam = Normalize(-viewDir);
          Xcam = Normalize(Cross(upVec, Zcam));
          Ycam = Normalize(Cross(Zcam, Xcam));
    }

    // Returns a matrix4x4 in row-major order and used with W.x style
    // Both world space and cam space are right-hand spaces
    Matrix4 world2cam() {
          // Renoarmlize
          Vector3f Xcam, Ycam, Zcam;
          renormalize(Xcam, Ycam, Zcam);
          // fill
          Matrix4 world2cam(Xcam.x, Xcam.y, Xcam.z, Dot(Vector3f(pos), Xcam),
                Ycam.x, Ycam.y, Ycam.z, Dot(Vector3f(pos), Ycam),
                Zcam.x, Zcam.y, Zcam.z, Dot(Vector3f(pos), Zcam),
                0, 0, 0, 1);
          return world2cam;
    }

    // Returns a row-major matrix4x4 and used with W.x style
    Matrix4 cam2world() {
          // Renoarmlize
          Vector3f Xcam, Ycam, Zcam;
          renormalize(Xcam, Ycam, Zcam);
          // fill
          Matrix4 cam2world(Xcam.x, Ycam.x, Zcam.x, pos.x,
                Xcam.y, Ycam.y, Zcam.y, pos.y,
                Xcam.z, Ycam.z, Zcam.z, pos.z,
                0.f,    0.f,    0.f,    1.f);
          return cam2world;
    }
};

class Camera {
    CameraState posAndOrientation;

};