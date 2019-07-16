#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vectors.h"
#include "matrix.h"
#include "ray.h"

#include "math.h"

class Camera {
  
  public:

    Camera() {}
    ~Camera() {}

    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;

};

class OrthographicCamera: public Camera {

  public:
    OrthographicCamera(Vec3f c, Vec3f d, Vec3f u, float s): center(c), size(s) {
      direction = d; direction.Normalize();
      up = u - direction * u.Dot3(direction); up.Normalize();
      horizontal = direction.Cross3(up); horizontal.Normalize();
    }
    
    virtual Ray generateRay(Vec2f point) {
      Vec3f p(center);
      p += horizontal * size * (point.x()-0.5) + up * size * (point.y()-0.5);
      Ray r(p, direction);
      return r;
    }

		virtual float getTMin() const { return 0.0; }

  protected:
    Vec3f center;
    Vec3f horizontal;
    Vec3f up;
    Vec3f direction;
    float size;
};


class PerspectiveCamera: public Camera {

  public:

    PerspectiveCamera(Vec3f c, Vec3f d, Vec3f u, float a): center(c), angle(a) {
      direction = d; direction.Normalize();
      up = u - direction * u.Dot3(direction); up.Normalize();
      horizontal = direction.Cross3(up); horizontal.Normalize();
      // convert angle to near plane(size=1), represented by direction
      float dis = 0.5 / tan(angle / 2);
      direction *= (float)sqrt(dis * dis - 0.5 * 0.5);
    }

    virtual Ray generateRay(Vec2f point) {
      Vec3f p(horizontal * (point.x()-0.5) + up * (point.y()-0.5));
      Vec3f dir = direction + p; dir.Normalize();
      Ray r(center, dir);
      return r;
    }

		virtual float getTMin() const { return 0.0; }

  protected:
    Vec3f center;
    Vec3f horizontal;
    Vec3f up;
    Vec3f direction;  // length equals distance to
                      // near plane with size = 1.0
    float angle;
};

#endif