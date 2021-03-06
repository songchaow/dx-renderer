#include "common/transform.h"

Matrix4 Translate(const Vector3f &delta) {
      Matrix4 m(1, 0, 0, delta.x, 0, 1, 0, delta.y, 0, 0, 1, delta.z, 0, 0, 0,
            1);
      return m;
}
Matrix4 Translate(const float x, const float y, const float z) {
      Matrix4 m(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
      return m;
}
Matrix4 Scale(float x, float y, float z) {
      Matrix4 m(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
      return m;
}

// Rotation order: rz.ry.rx.M
Matrix4 RotateM(float rx, float ry, float rz) {
      Matrix4 m;
      m[0][0] = std::cos(ry)*std::cos(rz);
      m[0][1] = std::cos(rz)*std::sin(rx)*std::sin(ry) - std::cos(rx)*std::sin(rz);
      m[0][2] = std::cos(rx)*std::cos(rz)*std::sin(ry) + std::sin(rx)*std::sin(rz);
      m[0][3] = 0.f;
      m[1][0] = std::cos(ry)*std::sin(rz);
      m[1][1] = std::cos(rx)*std::cos(rz) + std::sin(rx)*std::sin(ry)*std::sin(rz);
      m[1][2] = -std::cos(rz)* std::sin(rx) + std::cos(rx) * std::sin(ry) * std::sin(rz);
      m[1][3] = 0.f;
      m[2][0] = -std::sin(ry);
      m[2][1] = std::cos(ry)* std::sin(rx);
      m[2][2] = std::cos(rx)* std::cos(ry);
      m[2][3] = 0.f;
      m[3][0] = m[3][1] = m[3][2] = 0.f;
      m[3][3] = 1.f;
      return m;
}

Matrix4 _matrix4Identity;

const Matrix4& Matrix4::Identity() { return _matrix4Identity; }

Matrix3::Matrix3() {
      m_matrix[0][0] = m_matrix[1][1] = m_matrix[2][2] = 1.f;
      m_matrix[0][1] = m_matrix[0][2] = m_matrix[1][0] = m_matrix[1][2]
            = m_matrix[2][0] = m_matrix[2][1] = 0.f;
}

Matrix3::Matrix3(float t00, float t01, float t02,
      float t10, float t11, float t12,
      float t20, float t21, float t22) {
      m_matrix[0][0] = t00; m_matrix[0][1] = t01; m_matrix[0][2] = t02;
      m_matrix[1][0] = t10; m_matrix[1][1] = t11; m_matrix[1][2] = t22;
      m_matrix[2][0] = t20; m_matrix[2][1] = t21; m_matrix[2][2] = t22;
}

Matrix3::Matrix3(const Matrix4& m4) {
      m_matrix[0][0] = m4[0][0]; m_matrix[0][1] = m4[0][1]; m_matrix[0][2] = m4[0][2];
      m_matrix[1][0] = m4[1][0]; m_matrix[1][1] = m4[1][1]; m_matrix[1][2] = m4[1][2];
      m_matrix[2][0] = m4[2][0]; m_matrix[2][1] = m4[2][1]; m_matrix[2][2] = m4[2][2];
}

Matrix4::Matrix4() {
      m_matrix[0][0] = m_matrix[1][1] = m_matrix[2][2] = m_matrix[3][3] = 1.f;
      m_matrix[0][1] = m_matrix[0][2] = m_matrix[0][3] = m_matrix[1][0] = m_matrix[1][2] = m_matrix[1][3] = m_matrix[2][0] =
            m_matrix[2][1] = m_matrix[2][3] = m_matrix[3][0] = m_matrix[3][1] = m_matrix[3][2] = 0.f;
}

Matrix4::Matrix4(float t00, float t01, float t02, float t03, float t10,
      float t11, float t12, float t13, float t20, float t21,
      float t22, float t23, float t30, float t31, float t32,
      float t33) {
      m_matrix[0][0] = t00;
      m_matrix[0][1] = t01;
      m_matrix[0][2] = t02;
      m_matrix[0][3] = t03;
      m_matrix[1][0] = t10;
      m_matrix[1][1] = t11;
      m_matrix[1][2] = t12;
      m_matrix[1][3] = t13;
      m_matrix[2][0] = t20;
      m_matrix[2][1] = t21;
      m_matrix[2][2] = t22;
      m_matrix[2][3] = t23;
      m_matrix[3][0] = t30;
      m_matrix[3][1] = t31;
      m_matrix[3][2] = t32;
      m_matrix[3][3] = t33;
}

Matrix4::Matrix4(const Matrix3& m3) {
      m_matrix[0][0] = m3[0][0]; m_matrix[0][1] = m3[0][1]; m_matrix[0][2] = m3[0][2]; m_matrix[0][3] = 0.f;
      m_matrix[1][0] = m3[1][0]; m_matrix[1][1] = m3[1][1]; m_matrix[1][2] = m3[1][2]; m_matrix[1][3] = 0.f;
      m_matrix[2][0] = m3[2][0]; m_matrix[2][1] = m3[2][1]; m_matrix[2][2] = m3[2][2]; m_matrix[2][3] = 0.f;
      m_matrix[3][0] = 0.f; m_matrix[3][1] = 0.f; m_matrix[3][2] = 0.f; m_matrix[3][3] = 1.f;
}

Matrix4 SRT::toMatrix4() {
      Matrix4 translate = Translate(translation.x, translation.y, translation.z);
      // order: T.S.(rz.ry.rx).M
      Matrix4 rotate = RotateM(rotationX, rotationY, rotationZ);
      Matrix4 scale = Scale(scaleX, scaleY, scaleZ);
      return translate * scale * rotate;
}

Matrix4 Matrix4::operator*(const Matrix4& m) const {
      Matrix4 ret;
      for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                  ret[i][j] = 0.0f;
                  for (int t = 0; t < 4; t++)
                        ret[i][j] += m_matrix[i][t] * m[t][j];
            }
      return ret;
}

Vector3f Matrix4::operator()(const Vector3f & o) const
{
      Vector3f ret;
      ret.x = m_matrix[0][0] * o[0] + m_matrix[0][1] * o[1] + m_matrix[0][2] * o[2];
      ret.y = m_matrix[1][0] * o[0] + m_matrix[1][1] * o[1] + m_matrix[1][2] * o[2];
      ret.z = m_matrix[2][0] * o[0] + m_matrix[2][1] * o[1] + m_matrix[2][2] * o[2];
      return ret;
}


Point3f Matrix4::operator()(const Point3f & o) const {
      Point3f ret;
      ret.x = m_matrix[0][0] * o[0] + m_matrix[0][1] * o[1] + m_matrix[0][2] * o[2] + m_matrix[0][3];
      ret.y = m_matrix[1][0] * o[0] + m_matrix[1][1] * o[1] + m_matrix[1][2] * o[2] + m_matrix[1][3];
      ret.z = m_matrix[2][0] * o[0] + m_matrix[2][1] * o[1] + m_matrix[2][2] * o[2] + m_matrix[2][3];
      return ret;

}

SRT Matrix4::toSRT() const {
      SRT ret;
      // Translate
      ret.translation = { m_matrix[0][3], m_matrix[1][3], m_matrix[2][3] };
      ///start[0] = Matrix3(Translate(m_matrix[0][3], m_matrix[1][3], m_matrix[2][3]).m);
      // The order of scale and rotate doesn't matter
      // Scale
      ret.scaleX = Vector3f(m_matrix[0][0], m_matrix[1][0], m_matrix[2][0]).Length();
      ret.scaleY = Vector3f(m_matrix[0][1], m_matrix[1][1], m_matrix[2][1]).Length();
      ret.scaleZ = Vector3f(m_matrix[0][2], m_matrix[1][2], m_matrix[2][2]).Length();
      // Rotate TODO: check again!
      Matrix3 rotate = Matrix3(m_matrix[0][0] / ret.scaleX, m_matrix[0][1] / ret.scaleY, m_matrix[0][2] / ret.scaleZ,
            m_matrix[1][0] / ret.scaleX, m_matrix[1][1] / ret.scaleY, m_matrix[1][2] / ret.scaleZ,
            m_matrix[2][0] / ret.scaleX, m_matrix[2][1] / ret.scaleY, m_matrix[2][2] / ret.scaleZ);
      ret.rotationX = std::atan2(rotate[2][1], rotate[2][2]);
      ret.rotationY = std::atan2(-rotate[2][0], std::sqrt(rotate[2][1] * rotate[2][1] + rotate[2][2] * rotate[2][2]));
      ret.rotationZ = std::atan2(rotate[1][0], rotate[0][0]);

      return ret;
}

void Matrix4::transpose()
{
      for (int i = 0; i < 4; i++) {
            for (int j = i + 1; j < 4; j++) {
                  std::swap(m_matrix[i][j], m_matrix[j][i]);
            }
      }
}

Matrix4 Inverse(const Matrix4 &m) {
      int indxc[4], indxr[4];
      int ipiv[4] = { 0, 0, 0, 0 };
      float minv[4][4];
      memcpy(minv, m.m_matrix, 4 * 4 * sizeof(float));
      for (int i = 0; i < 4; i++) {
            int irow = 0, icol = 0;
            float big = 0.f;
            // Choose pivot
            for (int j = 0; j < 4; j++) {
                  if (ipiv[j] != 1) {
                        for (int k = 0; k < 4; k++) {
                              if (ipiv[k] == 0) {
                                    if (std::abs(minv[j][k]) >= big) {
                                          big = float(std::abs(minv[j][k]));
                                          irow = j;
                                          icol = k;
                                    }
                              }
                              else if (ipiv[k] > 1)
                                    LOG(ERROR) << "Singular matrix in MatrixInvert";
                        }
                  }
            }
            ++ipiv[icol];
            // Swap rows _irow_ and _icol_ for pivot
            if (irow != icol) {
                  for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
            }
            indxr[i] = irow;
            indxc[i] = icol;
            if (minv[icol][icol] == 0.f) LOG(ERROR) << "Singular matrix in MatrixInvert";

            // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
            float pivinv = 1. / minv[icol][icol];
            minv[icol][icol] = 1.;
            for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

            // Subtract this row from others to zero out their columns
            for (int j = 0; j < 4; j++) {
                  if (j != icol) {
                        float save = minv[j][icol];
                        minv[j][icol] = 0;
                        for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
                  }
            }
      }
      // Swap columns to reflect permutation
      for (int j = 3; j >= 0; j--) {
            if (indxr[j] != indxc[j]) {
                  for (int k = 0; k < 4; k++)
                        std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
            }
      }
      return Matrix4(minv);
}

AABB Matrix4::operator() (const AABB& aabb) const {
      Point3f points[8];
      points[0] = Point3f(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z);
      points[1] = Point3f(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z);
      points[2] = Point3f(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z);
      points[3] = Point3f(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z);
      points[4] = Point3f(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z);
      points[5] = Point3f(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z);
      points[6] = Point3f(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z);
      points[7] = Point3f(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z);
      for (uint16_t i = 0; i < 8; i++) {
            points[i] = operator()(points[i]);
      }
      Point3f pMax = points[0], pMin = points[0];
      for (uint16_t i = 1; i < 8; i++) {
            if (points[i].x > pMax.x)
                  pMax.x = points[i].x;
            if (points[i].y > pMax.y)
                  pMax.y = points[i].y;
            if (points[i].z > pMax.z)
                  pMax.z = points[i].z;
            if (points[i].x < pMin.x)
                  pMin.x = points[i].x;
            if (points[i].y < pMin.y)
                  pMin.y = points[i].y;
            if (points[i].z < pMin.z)
                  pMin.z = points[i].z;
      }
      return AABB(pMax, pMin);
}