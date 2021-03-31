#pragma once
#include "common/geometry.h"

struct Matrix4;

struct SRT {
      Point3f translation;
      float rotationX = 0.f, rotationY = 0.f, rotationZ = 0.f;
      float scaleX = 1.f, scaleY = 1.f, scaleZ = 1.f;
      Matrix4 toMatrix4();
      static SRT fromTranslation(const Point3f& t) { SRT ret; ret.translation = t; return ret; }
      static SRT fromTranslation(float x, float y, float z) {
            SRT ret;
            ret.translation.x = x;
            ret.translation.y = y;
            ret.translation.z = z;
            return ret;
      }
};


struct Matrix3 {
      float m_matrix[3][3];
      Matrix3();
      Matrix3(float t00, float t01, float t02,
            float t10, float t11, float t12,
            float t20, float t21, float t22);
      Matrix3(const Matrix4& m4);
      float* operator[](const int n) { return m_matrix[n]; }
      const float* operator[](const int n) const { return m_matrix[n]; }
};

struct Matrix4 {
      float m_matrix[4][4];
      Matrix4();
      Matrix4(float t00, float t01, float t02, float t03, float t10,
            float t11, float t12, float t13, float t20, float t21,
            float t22, float t23, float t30, float t31, float t32,
            float t33);
      Matrix4(float(*m)[4]) { memcpy(m_matrix, m, 4 * 4 * sizeof(float)); }
      // convert from Matrix3
      Matrix4(const Matrix3& m3);
      float* operator[](const int n) { return m_matrix[n]; }
      const float* operator[](const int n) const { return m_matrix[n]; }
      Matrix4 operator*(const Matrix4& m) const;
      Vector3f operator()(const Vector3f& v) const;
      Point3f operator()(const Point3f& p) const;
      AABB operator() (const AABB& aabb) const;
      SRT toSRT() const;
      void transpose();
      static const Matrix4& Identity();
};

Matrix4 Translate(const Vector3f &delta);
Matrix4 Translate(const float x, const float y, const float z);
Matrix4 Scale(float x, float y, float z);

struct Quaternion {
      // a + bi + cj + dk
      float a; // also s
      float b;
      float c;
      float d;
      Quaternion() = default;
      Quaternion(float a, float b, float c, float d)
            : a(a), b(b), c(c), d(d) {}
      Quaternion(Vector3f spinAxis, float theta) {
            Normalize(spinAxis);
            a = std::cos(theta);
            float sinT = std::sin(theta);
            b = sinT * spinAxis.x;
            c = sinT * spinAxis.y;
            d = sinT * spinAxis.z;
      }
      Quaternion operator*(const Quaternion& rhs) const {
            const float& e = rhs.a;
            const float& f = rhs.b;
            const float& g = rhs.c;
            const float& h = rhs.d;
            float newa = a * e - b * f - c * g - d * h;
            float newb = a * f + b * e + c * h - d * g;
            float newc = a * g - b * h + c * e + d * f;
            float newd = a * h + b * g - c * f + d * e;
            Normalize(newa, newb, newc, newd);
            return Quaternion(newa, newb, newc, newd);
      }
      Quaternion Inverse() const {
            // assume the quaternion normalized?
            return Quaternion(a, -b, -c, -d);
      }
      Matrix4 toMatrix4() const {
            return Matrix4(a*a + b * b - c * c - d * d, 2 * b*c - 2 * a*d, 2 * b*d + 2 * a*c, 0,
                  2 * b*c + 2 * a*d, a*a - b * b + c * c - d * d, 2 * c*d - 2 * a*b, 0,
                  2 * b*d - 2 * a*c, 2 * c*d + 2 * a*b, a*a - b * b - c * c + d * d, 0,
                  0, 0, 0, 1);
      }
      Vector3f operator*(const Vector3f& rhs) {
            Quaternion rhs_q(0, rhs.x, rhs.y, rhs.z);
            Quaternion res = *this * rhs_q * Inverse();
            return Vector3f(res.b, res.c, res.d);
      }

};