#include "geometry.h"

bool CamOrientedEllipse::inRange(const Point2f& xzCam) const {
      Point2f xzLocal = Point2f(xzCam - center);
      // in xz, x is longer
      float val = axisY * axisY * xzCam.x * xzCam.x + axisX * axisX * xzCam.y * xzCam.y;
      float range_val = axisX * axisX * axisY * axisY;
      return val < range_val;
}

bool CamOrientedEllipse::inRange(const Point2f& xzCam, Location& loc) const {
      Point2f xzLocal = Point2f(xzCam - center);
      // in xz, x is longer
      float val = axisY * axisY * xzLocal.x * xzLocal.x + axisX * axisX * xzLocal.y * xzLocal.y;
      float range_val = axisX * axisX * axisY * axisY;
      loc = xzLocal.y > 0 ? FRONT : BACK;
      return val < range_val;
}