#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vectors.h"
#include "ray.h"

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

    virtual float getTMin() const {

    }

  protected:
    Vec3f center;
    Vec3f horizontal;
    Vec3f up;
    Vec3f direction;
    float size;

};


#endif