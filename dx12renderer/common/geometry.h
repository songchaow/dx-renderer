
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

constexpr float Pi = 3.1415927f;

#include <iterator>
#include <glog/logging.h>

template <typename T, typename T1, typename T2> T Clamp(T val, T1 min, T2 max) {
      if (val < min) return min;
      if (val > max) return max;
      return val;
}

template <typename T>
inline bool isNaN(const T x) {
    return std::isnan(x);
}
template <>
inline bool isNaN(const int x) {
    return false;
}
template <typename T>
class Vector2;
template <typename T>
class Vector3;
template <typename T>
class Point3;
template <typename T>
class Point2;
template <typename T>
class Normal3;


// Point Declarations
template <typename T>
class Point3 {
  public:
    // Point3 Public Methods
    Point3() { x = y = z = 0; }
    Point3(T x, T y, T z) : x(x), y(y), z(z) { DCHECK(!HasNaNs()); }
    template <typename U>
    explicit Point3(const Point3<U> &p)
        : x((T)p.x), y((T)p.y), z((T)p.z) {
        DCHECK(!HasNaNs());
    }
    template <typename U>
    explicit Point3(const Vector3<U> &p)
          : x((T)p.x), y((T)p.y), z((T)p.z) {
          DCHECK(!HasNaNs());
    }
    template <typename U>
    explicit operator Vector3<U>() const {
        return Vector3<U>(x, y, z);
    }
#ifndef NDEBUG
    Point3(const Point3<T> &p) {
        DCHECK(!p.HasNaNs());
        x = p.x;
        y = p.y;
        z = p.z;
    }

    Point3<T> &operator=(const Point3<T> &p) {
        DCHECK(!p.HasNaNs());
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }
#endif  // !NDEBUG
    Point3<T> operator+(const Vector3<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Point3<T>(x + v.x, y + v.y, z + v.z);
    }
    Point3<T> &operator+=(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3<T> operator-(const Point3<T> &p) const {
        DCHECK(!p.HasNaNs());
        return Vector3<T>(x - p.x, y - p.y, z - p.z);
    }
    Point3<T> operator-(const Vector3<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Point3<T>(x - v.x, y - v.y, z - v.z);
    }
    Point3<T> &operator-=(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    Point3<T> &operator+=(const Point3<T> &p) {
        DCHECK(!p.HasNaNs());
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }
    Point3<T> operator+(const Point3<T> &p) const {
        DCHECK(!p.HasNaNs());
        return Point3<T>(x + p.x, y + p.y, z + p.z);
    }
    template <typename U>
    Point3<T> operator*(U f) const {
        return Point3<T>(f * x, f * y, f * z);
    }
    template <typename U>
    Point3<T> &operator*=(U f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    template <typename U>
    Point3<T> operator/(U f) const {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        return Point3<T>(inv * x, inv * y, inv * z);
    }
    template <typename U>
    Point3<T> &operator/=(U f) {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
    Point3<T> &operator/=(const Point3<T>& rhs) {
          x /= rhs.x;
          y /= rhs.y;
          z /= rhs.z;
          return *this;
    }

    Point3<T> &operator/=(const Vector3<T>& rhs) {
          x /= rhs.x;
          y /= rhs.y;
          z /= rhs.z;
          return *this;
    }

    T operator[](int i) const {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    T &operator[](int i) {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    bool operator==(const Point3<T> &p) const {
        return x == p.x && y == p.y && z == p.z;
    }
    bool operator!=(const Point3<T> &p) const {
        return x != p.x || y != p.y || z != p.z;
    }
    bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
    Point3<T> operator-() const { return Point3<T>(-x, -y, -z); }

    // Point3 Public Data
    T x, y, z;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Point3<float> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

template <typename T>
class Point2 {
  public:
    // Point2 Public Methods
    explicit Point2(const Point3<T> &p) : x(p.x), y(p.y) { DCHECK(!HasNaNs()); }
    Point2() { x = y = 0; }
    Point2(T xx, T yy) : x(xx), y(yy) { DCHECK(!HasNaNs()); }

    template <typename U>
    explicit Point2(const Point2<U> &p) {
        x = (T)p.x;
        y = (T)p.y;
        DCHECK(!HasNaNs());
    }

    template <typename U>
    explicit Point2(const Vector2<U> &p) {
        x = (T)p.x;
        y = (T)p.y;
        DCHECK(!HasNaNs());
    }

    template <typename U>
    explicit operator Vector2<U>() const {
        return Vector2<U>(x, y);
    }

#ifndef NDEBUG
    Point2(const Point2<T> &p) {
        DCHECK(!p.HasNaNs());
        x = p.x;
        y = p.y;
    }

    Point2<T> &operator=(const Point2<T> &p) {
        DCHECK(!p.HasNaNs());
        x = p.x;
        y = p.y;
        return *this;
    }
#endif  // !NDEBUG
    Point2<T> operator+(const Vector2<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Point2<T>(x + v.x, y + v.y);
    }

    Point2<T> &operator+=(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2<T> operator-(const Point2<T> &p) const {
        DCHECK(!p.HasNaNs());
        return Vector2<T>(x - p.x, y - p.y);
    }

    Point2<T> operator-(const Vector2<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Point2<T>(x - v.x, y - v.y);
    }
    Point2<T> operator-(const float v) const {
          return Point2<T>(x - v, y - v);
    }
    Point2<T> operator+(const float v) const {
          return Point2<T>(x + v, y + v);
    }
    Point2<T> operator-() const { return Point2<T>(-x, -y); }
    Point2<T> &operator-=(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Point2<T> &operator+=(const Point2<T> &p) {
        DCHECK(!p.HasNaNs());
        x += p.x;
        y += p.y;
        return *this;
    }
    Point2<T> operator+(const Point2<T> &p) const {
        DCHECK(!p.HasNaNs());
        return Point2<T>(x + p.x, y + p.y);
    }
    template <typename U>
    Point2<T> operator*(U f) const {
        return Point2<T>(f * x, f * y);
    }
    template <typename U>
    Point2<T> &operator*=(U f) {
        x *= f;
        y *= f;
        return *this;
    }
    template <typename U>
    Point2<T> operator/(U f) const {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        return Point2<T>(inv * x, inv * y);
    }
    template <typename U>
    Point2<T> &operator/=(U f) {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        x *= inv;
        y *= inv;
        return *this;
    }
    Point2<T> &operator/=(const Point2<T>& rhs) {
          
          x /= rhs.x;
          y /= rhs.y;
          return *this;
    }

    T operator[](int i) const {
        DCHECK(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }

    T &operator[](int i) {
        DCHECK(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }
    bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point2<T> &p) const { return x != p.x || y != p.y; }
    bool HasNaNs() const { return isNaN(x) || isNaN(y); }

    // Point2 Public Data
    T x, y;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point2<T> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Point2<float> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

typedef Point2<float> Point2f;
typedef Point2<int> Point2i;
typedef Point3<float> Point3f;
typedef Point3<int> Point3i;

// Normal Declarations
template <typename T>
class Normal3 {
  public:
    // Normal3 Public Methods
    Normal3() { x = y = z = 0; }
    Normal3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { DCHECK(!HasNaNs()); }
    Normal3<T> operator-() const { return Normal3(-x, -y, -z); }
    Normal3<T> operator+(const Normal3<T> &n) const {
        DCHECK(!n.HasNaNs());
        return Normal3<T>(x + n.x, y + n.y, z + n.z);
    }

    Normal3<T> &operator+=(const Normal3<T> &n) {
        DCHECK(!n.HasNaNs());
        x += n.x;
        y += n.y;
        z += n.z;
        return *this;
    }
    Normal3<T> operator-(const Normal3<T> &n) const {
        DCHECK(!n.HasNaNs());
        return Normal3<T>(x - n.x, y - n.y, z - n.z);
    }

    Normal3<T> &operator-=(const Normal3<T> &n) {
        DCHECK(!n.HasNaNs());
        x -= n.x;
        y -= n.y;
        z -= n.z;
        return *this;
    }
    bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
    template <typename U>
    Normal3<T> operator*(U f) const {
        return Normal3<T>(f * x, f * y, f * z);
    }

    template <typename U>
    Normal3<T> &operator*=(U f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    template <typename U>
    Normal3<T> operator/(U f) const {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        return Normal3<T>(x * inv, y * inv, z * inv);
    }

    template <typename U>
    Normal3<T> &operator/=(U f) {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(LengthSquared()); }

#ifndef NDEBUG
    Normal3<T>(const Normal3<T> &n) {
        DCHECK(!n.HasNaNs());
        x = n.x;
        y = n.y;
        z = n.z;
    }
    Normal3<T>(const Vector3<T> &n) {
          DCHECK(!n.HasNaNs());
          x = n.x;
          y = n.y;
          z = n.z;
    }
    Normal3<T>(const Point3<T> &n) {
          DCHECK(!n.HasNaNs());
          x = n.x;
          y = n.y;
          z = n.z;
    }

    Normal3<T> &operator=(const Normal3<T> &n) {
        DCHECK(!n.HasNaNs());
        x = n.x;
        y = n.y;
        z = n.z;
        return *this;
    }
#endif  // !NDEBUG

#ifdef NDEBUG
    explicit Normal3<T>(const Vector3<T> &v) : x(v.x), y(v.y), z(v.z) {
        DCHECK(!v.HasNaNs());
    }
    bool operator==(const Normal3<T> &n) const {
        return x == n.x && y == n.y && z == n.z;
    }
    bool operator!=(const Normal3<T> &n) const {
        return x != n.x || y != n.y || z != n.z;
    }
#endif

    T operator[](int i) const {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    T &operator[](int i) {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    // Normal3 Public Data
    T x, y, z;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Normal3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Normal3<float> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

typedef Normal3<float> Normal3f;

// Vector Declarations
template <typename T>
class Vector2 {
  public:
    // Vector2 Public Methods
    Vector2() { x = y = 0; }
    Vector2(T xx, T yy) : x(xx), y(yy) { DCHECK(!HasNaNs()); }
    bool HasNaNs() const { return isNaN(x) || isNaN(y); }
    explicit Vector2(const Point2<T> &p);
    explicit Vector2(const Point3<T> &p);
#ifndef NDEBUG
    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the Assert checks.
    Vector2(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
    }
    Vector2<T> &operator=(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
        return *this;
    }
#endif  // !NDEBUG

    Vector2<T> operator+(const Vector2<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Vector2(x + v.x, y + v.y);
    }

    Vector2<T> &operator+=(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2<T> operator-(const Vector2<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Vector2(x - v.x, y - v.y);
    }

    Vector2<T> &operator-=(const Vector2<T> &v) {
        DCHECK(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        return *this;
    }
    bool operator==(const Vector2<T> &v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vector2<T> &v) const { return x != v.x || y != v.y; }
    template <typename U>
    Vector2<T> operator*(U f) const {
        return Vector2<T>(f * x, f * y);
    }

    template <typename U>
    Vector2<T> &operator*=(U f) {
        DCHECK(!isNaN(f));
        x *= f;
        y *= f;
        return *this;
    }
    template <typename U>
    Vector2<T> operator/(U f) const {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        return Vector2<T>(x * inv, y * inv);
    }

    template <typename U>
    Vector2<T> &operator/=(U f) {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        x *= inv;
        y *= inv;
        return *this;
    }
    Vector2<T> operator-() const { return Vector2<T>(-x, -y); }
    T operator[](int i) const {
        DCHECK(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }

    T &operator[](int i) {
        DCHECK(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }
    float LengthSquared() const { return x * x + y * y; }
    float Length() const { return std::sqrt(LengthSquared()); }

    // Vector2 Public Data
    T x, y;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Vector2<float> &v) {
    os << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

template <typename T>
class Vector3 {
  public:
    // Vector3 Public Methods
    T operator[](int i) const {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    T &operator[](int i) {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    Vector3() { x = y = z = 0; }
    Vector3(T t) { x = y = z = t; }
    Vector3(T x, T y, T z) : x(x), y(y), z(z) { DCHECK(!HasNaNs()); }
    Vector3(std::initializer_list<T> i) {
          x = *i.begin();
          y = *(i.begin() + 1);
          z = *(i.begin() + 2);
    }
    bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
    explicit Vector3(const Point3<T> &p);
#ifndef NDEBUG
    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the Assert checks.
    Vector3(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector3<T> &operator=(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
#endif  // !NDEBUG
    Vector3<T> operator+(const Vector3<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    Vector3<T> &operator+=(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3<T> operator-(const Vector3<T> &v) const {
        DCHECK(!v.HasNaNs());
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    Vector3<T> &operator-=(const Vector3<T> &v) {
        DCHECK(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    bool operator==(const Vector3<T> &v) const {
        return x == v.x && y == v.y && z == v.z;
    }
    bool operator!=(const Vector3<T> &v) const {
        return x != v.x || y != v.y || z != v.z;
    }
    template <typename U>
    Vector3<T> operator*(U s) const {
        return Vector3<T>(s * x, s * y, s * z);
    }
    template <typename U>
    Vector3<T> &operator*=(U s) {
        DCHECK(!isNaN(s));
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    template <typename U>
    Vector3<T> operator/(U f) const {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        return Vector3<T>(x * inv, y * inv, z * inv);
    }

    template <typename U>
    Vector3<T> &operator/=(U f) {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
    Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(LengthSquared()); }
    explicit Vector3(const Normal3<T> &n);

    // Vector3 Public Data
    T x, y, z;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

template <>
inline std::ostream &operator<<(std::ostream &os, const Vector3<float> &v) {
    os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

// Geometry Inline Functions
template <typename T>
inline Vector3<T>::Vector3(const Point3<T> &p)
    : x(p.x), y(p.y), z(p.z) {
    DCHECK(!HasNaNs());
}

template <typename T, typename U>
inline Vector3<T> operator*(U s, const Vector3<T> &v) {
    return v * s;
}
template <typename T>
Vector3<T> Abs(const Vector3<T> &v) {
    return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

template <typename T>
inline T Dot(const Vector3<T> &v1, const Vector3<T> &v2) {
    DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
inline T AbsDot(const Vector3<T> &v1, const Vector3<T> &v2) {
    DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
    return std::abs(Dot(v1, v2));
}

template <typename T>
inline Vector3<T> Cross(const Vector3<T> &v1, const Vector3<T> &v2) {
    DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
    double v1x = v1.x, v1y = v1.y, v1z = v1.z;
    double v2x = v2.x, v2y = v2.y, v2z = v2.z;
    return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
                      (v1x * v2y) - (v1y * v2x));
}

template <typename T>
inline Vector3<T> Cross(const Vector3<T> &v1, const Normal3<T> &v2) {
    DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
    double v1x = v1.x, v1y = v1.y, v1z = v1.z;
    double v2x = v2.x, v2y = v2.y, v2z = v2.z;
    return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
                      (v1x * v2y) - (v1y * v2x));
}

template <typename T>
inline Vector3<T> Cross(const Normal3<T> &v1, const Vector3<T> &v2) {
    DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
    double v1x = v1.x, v1y = v1.y, v1z = v1.z;
    double v2x = v2.x, v2y = v2.y, v2z = v2.z;
    return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
                      (v1x * v2y) - (v1y * v2x));
}

template <typename T>
inline Vector3<T> Normalize(const Vector3<T> &v) {
    return v / v.Length();
}
template <typename T>
inline void Normalize(T &a, T &b, T &c) {
      T length = std::sqrt(a*a + b * b + c * c);
      a /= length;
      b /= length;
      c /= length;
}
template <typename T>
inline void Normalize(T &a, T &b, T &c, T &d) {
    T length = std::sqrt(a*a+b*b+c*c+d*d);
    a /= length;
    b /= length;
    c /= length;
    d /= length; 
}
template <typename T>
T MinComponent(const Vector3<T> &v) {
    return std::min(v.x, std::min(v.y, v.z));
}

template <typename T>
T MaxComponent(const Vector3<T> &v) {
    return std::max(v.x, std::max(v.y, v.z));
}

template <typename T>
int MaxDimension(const Vector3<T> &v) {
    return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}

template <typename T>
Vector3<T> Min(const Vector3<T> &p1, const Vector3<T> &p2) {
    return Vector3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                      std::min(p1.z, p2.z));
}

template <typename T>
Vector3<T> Max(const Vector3<T> &p1, const Vector3<T> &p2) {
    return Vector3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                      std::max(p1.z, p2.z));
}

template <typename T>
Vector3<T> Permute(const Vector3<T> &v, int x, int y, int z) {
    return Vector3<T>(v[x], v[y], v[z]);
}

template <typename T>
inline void CoordinateSystem(const Vector3<T> &v1, Vector3<T> *v2,
                             Vector3<T> *v3) {
    if (std::abs(v1.x) > std::abs(v1.y))
        *v2 = Vector3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
    else
        *v2 = Vector3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
    *v3 = Cross(v1, *v2);
}

template <typename T>
Vector2<T>::Vector2(const Point2<T> &p)
    : x(p.x), y(p.y) {
    DCHECK(!HasNaNs());
}

template <typename T>
Vector2<T>::Vector2(const Point3<T> &p)
    : x(p.x), y(p.y) {
    DCHECK(!HasNaNs());
}

template <typename T, typename U>
inline Vector2<T> operator*(U f, const Vector2<T> &v) {
    return v * f;
}
template <typename T>
inline float Dot(const Vector2<T> &v1, const Vector2<T> &v2) {
    DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
    return v1.x * v2.x + v1.y * v2.y;
}

template <typename T>
inline float AbsDot(const Vector2<T> &v1, const Vector2<T> &v2) {
    DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
    return std::abs(Dot(v1, v2));
}

template <typename T>
inline Vector2<T> Normalize(const Vector2<T> &v) {
    return v / v.Length();
}
template <typename T>
Vector2<T> Abs(const Vector2<T> &v) {
    return Vector2<T>(std::abs(v.x), std::abs(v.y));
}

template <typename T>
inline float Distance(const Point3<T> &p1, const Point3<T> &p2) {
    return (p1 - p2).Length();
}

template <typename T>
inline float DistanceSquared(const Point3<T> &p1, const Point3<T> &p2) {
    return (p1 - p2).LengthSquared();
}

template <typename T, typename U>
inline Point3<T> operator*(U f, const Point3<T> &p) {
    DCHECK(!p.HasNaNs());
    return p * f;
}

template <typename T>
Point3<T> Lerp(float t, const Point3<T> &p0, const Point3<T> &p1) {
    return (1 - t) * p0 + t * p1;
}

template <typename T>
Point3<T> Min(const Point3<T> &p1, const Point3<T> &p2) {
    return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                     std::min(p1.z, p2.z));
}

template <typename T>
Point3<T> Max(const Point3<T> &p1, const Point3<T> &p2) {
    return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                     std::max(p1.z, p2.z));
}

template <typename T>
Point3<T> Floor(const Point3<T> &p) {
    return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z));
}

template <typename T>
Point3<T> Ceil(const Point3<T> &p) {
    return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z));
}

template <typename T>
Point3<T> Abs(const Point3<T> &p) {
    return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
}

template <typename T>
inline float Distance(const Point2<T> &p1, const Point2<T> &p2) {
    return (p1 - p2).Length();
}

template <typename T>
inline float DistanceSquared(const Point2<T> &p1, const Point2<T> &p2) {
    return (p1 - p2).LengthSquared();
}

template <typename T, typename U>
inline Point2<T> operator*(U f, const Point2<T> &p) {
    DCHECK(!p.HasNaNs());
    return p * f;
}

template <typename T>
Point2<T> Floor(const Point2<T> &p) {
    return Point2<T>(std::floor(p.x), std::floor(p.y));
}

template <typename T>
Point2<T> Ceil(const Point2<T> &p) {
    return Point2<T>(std::ceil(p.x), std::ceil(p.y));
}

template <typename T>
Point2<T> Lerp(float t, const Point2<T> &v0, const Point2<T> &v1) {
    return (1 - t) * v0 + t * v1;
}

template <typename T>
Point2<T> Min(const Point2<T> &pa, const Point2<T> &pb) {
    return Point2<T>(std::min(pa.x, pb.x), std::min(pa.y, pb.y));
}

template <typename T>
Point2<T> Max(const Point2<T> &pa, const Point2<T> &pb) {
    return Point2<T>(std::max(pa.x, pb.x), std::max(pa.y, pb.y));
}

template <typename T>
Point3<T> Permute(const Point3<T> &p, int x, int y, int z) {
    return Point3<T>(p[x], p[y], p[z]);
}

template <typename T, typename U>
inline Normal3<T> operator*(U f, const Normal3<T> &n) {
    return Normal3<T>(f * n.x, f * n.y, f * n.z);
}

template <typename T>
inline Normal3<T> Normalize(const Normal3<T> &n) {
    return n / n.Length();
}

template <typename T>
inline Vector3<T>::Vector3(const Normal3<T> &n)
    : x(n.x), y(n.y), z(n.z) {
    DCHECK(!n.HasNaNs());
}

template <typename T>
inline T Dot(const Normal3<T> &n1, const Vector3<T> &v2) {
    DCHECK(!n1.HasNaNs() && !v2.HasNaNs());
    return n1.x * v2.x + n1.y * v2.y + n1.z * v2.z;
}

template <typename T>
inline T Dot(const Vector3<T> &v1, const Normal3<T> &n2) {
    DCHECK(!v1.HasNaNs() && !n2.HasNaNs());
    return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
}

template <typename T>
inline T Dot(const Normal3<T> &n1, const Normal3<T> &n2) {
    DCHECK(!n1.HasNaNs() && !n2.HasNaNs());
    return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}

template <typename T>
inline T AbsDot(const Normal3<T> &n1, const Vector3<T> &v2) {
    DCHECK(!n1.HasNaNs() && !v2.HasNaNs());
    return std::abs(n1.x * v2.x + n1.y * v2.y + n1.z * v2.z);
}

template <typename T>
inline T AbsDot(const Vector3<T> &v1, const Normal3<T> &n2) {
    DCHECK(!v1.HasNaNs() && !n2.HasNaNs());
    return std::abs(v1.x * n2.x + v1.y * n2.y + v1.z * n2.z);
}

template <typename T>
inline T AbsDot(const Normal3<T> &n1, const Normal3<T> &n2) {
    DCHECK(!n1.HasNaNs() && !n2.HasNaNs());
    return std::abs(n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
}

template <typename T>
inline Normal3<T> Faceforward(const Normal3<T> &n, const Vector3<T> &v) {
    return (Dot(n, v) < 0.f) ? -n : n;
}

template <typename T>
inline Normal3<T> Faceforward(const Normal3<T> &n, const Normal3<T> &n2) {
    return (Dot(n, n2) < 0.f) ? -n : n;
}

template <typename T>
inline Vector3<T> Faceforward(const Vector3<T> &v, const Vector3<T> &v2) {
    return (Dot(v, v2) < 0.f) ? -v : v;
}

template <typename T>
inline Vector3<T> Faceforward(const Vector3<T> &v, const Normal3<T> &n2) {
    return (Dot(v, n2) < 0.f) ? -v : v;
}

template <typename T>
Normal3<T> Abs(const Normal3<T> &v) {
    return Normal3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

inline Vector3f SphericalDirection(float sinTheta, float cosTheta, float phi) {
    return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi),
                    cosTheta);
}

inline Vector3f SphericalDirection(float sinTheta, float cosTheta, float phi,
                                   const Vector3f &x, const Vector3f &y,
                                   const Vector3f &z) {
    return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y +
           cosTheta * z;
}

inline float CaconicalSphericalTheta(const Vector3f &v) {
      return std::acos(Clamp(v.z, -1, 1));
}

inline float SphericalTheta(const Vector3f &v) {
    return std::acos(Clamp(v.y, -1, 1));
}

inline float CaconicalSphericalPhi(const Vector3f &v) {
    float p = std::atan2(v.y, v.x);
    return (p < 0) ? (p + 2 * Pi) : p;
}

/* Cheat Sheet for phi(x,z):
      x>0, z>0: res = res
      x<0, z>0: res += pi
      x<0, z<0: res += pi
      x>0, z<0: res += 2pi
      final p is in [0,2Pi]
*/
inline float SphericalPhi(const Vector3f &v) {
      float p = std::atan2(v.z, v.x);
      //if (v.x < 0)
      //      ;// p += Pi;
      //if (v.x > 0 && v.z < 0)
      //      p += 2 * Pi;
      if (v.y < 0)
            p += 2 * Pi;
      return p;
}

inline bool SameOpposition(const Normal3f& n, const Vector3f& wi) {
      return Dot(n, wi) > 0;
}

// Axis aligned bounding box
struct AABB {
      Point3f pMax;
      Point3f pMin;
      AABB() {
            float max = std::numeric_limits<float>::max();
            float min = std::numeric_limits<float>::lowest();
            pMin = { max, max, max };
            pMax = { min,min,min };
      }
      AABB(const Point3f& initPoint) : pMax(initPoint), pMin(initPoint) {}
      AABB(const Point3f& pMax, const Point3f& pMin) : pMax(pMax), pMin(pMin) {}
      void Add(const Point3f& p) {
            if (p.x > pMax.x)
                  pMax.x = p.x;
            if (p.y > pMax.y)
                  pMax.y = p.y;
            if (p.z > pMax.z)
                  pMax.z = p.z;
            if (p.x < pMin.x)
                  pMin.x = p.x;
            if (p.y < pMin.y)
                  pMin.y = p.y;
            if (p.z < pMin.z)
                  pMin.z = p.z;
      }
      Point3f center() const { return 0.5 * (pMax + pMin); }
      Vector3f extent() const { return (pMax - pMin); }
      Point3f& operator[](uint8_t i) { return i == 0 ? pMax : pMin; }
      const Point3f& operator[](uint8_t i) const { return i == 0 ? pMax : pMin; }
      AABB& operator+=(const AABB& rhs) {
            Add(rhs.pMax);
            Add(rhs.pMin);
            return *this;
      }
      bool In(const Point3f& p) const {
            return p.x<pMax.x && p.y<pMax.y && p.z<pMax.z &&
                  p.x>pMin.x && p.y>pMin.y && p.z> pMin.z;
      }
};

struct SphereBound {
      Point3f center;
      float radius;
};

struct CamOrientedEllipse {
      float axisX; // long
      float axisY; // short
      Point2f center;
      bool inRange(const Point2f& xzCam) const;
      enum Location {
            FRONT,
            BACK
      };
      bool inRange(const Point2f& xzCam, Location& loc) const;
      float FrontZ() const { return center.y + axisY; }
      float BackZ() const { return center.y - axisY; }
};