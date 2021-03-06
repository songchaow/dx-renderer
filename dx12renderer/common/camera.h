#pragma once
#include "geometry.h"
#include "common/transform.h"

struct ViewVec {
      float phi = 0.f;
      float theta = Pi/2; // start from

};

struct CameraState {
    Point3f pos;
    Vector3f viewDir;
    Vector3f Y;
    
    mutable Matrix4 _world2cam_cache;
    mutable bool _world2cam_dirty = true;

    void renormalize(Vector3f& Xcam, Vector3f& Ycam, Vector3f& Zcam) const {
          Zcam = Normalize(-viewDir);
          Xcam = Normalize(Cross(Y, Zcam));
          Ycam = Normalize(Cross(Zcam, Xcam));
    }

    // Returns a matrix4x4 in row-major order and used with W.x style
    // Both world space and cam space are right-hand spaces
    Matrix4 _world2cam() const {
          /// Renoarmlize
          Vector3f Zcam = -viewDir;
          Vector3f Xcam = Cross(Y, -viewDir);

          // fill
          Matrix4 world2cam(Xcam.x, Xcam.y, Xcam.z, Dot(Vector3f(pos), Xcam),
                Y.x, Y.y, Y.z, Dot(Vector3f(pos), Y),
                Zcam.x, Zcam.y, Zcam.z, Dot(Vector3f(pos), Zcam),
                0, 0, 0, 1);
          return world2cam;
    }

    // The cached version
    Matrix4 world2cam() const {
          if (_world2cam_dirty) {
                _world2cam_cache = _world2cam();
                _world2cam_dirty = false;
          }
          return _world2cam_cache;
    }

    // Returns a row-major matrix4x4 and used with W.x style
    Matrix4 cam2world() const {
          // Renoarmlize
          Vector3f Zcam = -viewDir;
          Vector3f Xcam = Cross(Y, -viewDir);
          // fill
          Matrix4 cam2world(Xcam.x, Y.x, Zcam.x, pos.x,
                Xcam.y, Y.y, Zcam.y, pos.y,
                Xcam.z, Y.z, Zcam.z, pos.z,
                0.f,    0.f,    0.f,    1.f);
          return cam2world;
    }
    void forward(float d) {
          pos += viewDir * d;
          _world2cam_dirty = true;
    }

    // Moves the pos along the X axis
    void horizonMove(float d) {
          Vector3f R = Normalize(Cross(Y, -viewDir));
          pos += R * d;
          _world2cam_dirty = true;
    }

    // Moves the pos along the Y axis
    void verticalMove(float d) {
          pos += Y * d;
          _world2cam_dirty = true;
    }

    // Rotates the view vector
    void scroll(float dx, float dy) {
          // dx controls the rotation along Y
          // assume already normalized
          Vector3f X = Cross(Y, -viewDir);
          Y = Normalize(Cross(-viewDir, X));
          Quaternion rotate_dx(Y, dx);
          Quaternion rotate_dy(X, dy);
          viewDir = (rotate_dx * rotate_dy).toMatrix4()(viewDir);
          // renormalize
          viewDir = Normalize(viewDir);
          X = Cross(Vector3f(0, 1, 0), -viewDir);
          Y = Normalize(Cross(-viewDir, X));
          _world2cam_dirty = true;
    }
};


struct Frustum {
      enum FrustumType {
            Projective, Orthogonal
      };
      FrustumType type;
      float Near;
      float Far;
      // Orthogonal
      float width;
      float height; // not used
      // Prospective
      float aspectRatio;
      float fov_Horizontal;
      // default Prospective ctor
      Frustum(float aspectRatio) : Near(0.1f), Far(200.f), aspectRatio(aspectRatio),
            fov_Horizontal(90.f * Pi / 180), type(Projective) {}
      /*Frustum(float aspectRatio) : near(1.f), Far(1500.f), aspectRatio(aspectRatio),
            fov_Horizontal(90.f * Pi / 180), type(Projective) {}*/
      Frustum() : Frustum(1.6f) {}
      Frustum(float width, float height, float length) : width(width), height(height), Near(0.f),
            Far(length), type(Orthogonal) {}
      Matrix4 cam2ndc_Perspective() const;
      Matrix4 cam2ndc_Orthogonal() const;
      Matrix4 cam2ndc() const { return type == Projective ? cam2ndc_Perspective() : cam2ndc_Orthogonal(); }
      
};

struct View {
      Frustum f;
      Matrix4 world2view;
      Matrix4 world2ndc; // cached
      View() = default;
      View(const Frustum& f, const Matrix4& m) : f(f), world2view(m) {}
};

class Camera {
      CameraState posAndOrientation;
      Frustum frustum;

public:
      Matrix4 world2cam() const { return posAndOrientation.world2cam(); }
      bool world2cam_dirty() const { return posAndOrientation._world2cam_dirty; }
      Matrix4 cam2world() const { return posAndOrientation.cam2world(); }
      Matrix4 cam2ndc() const { return frustum.cam2ndc(); }
};

extern Camera g_camera;