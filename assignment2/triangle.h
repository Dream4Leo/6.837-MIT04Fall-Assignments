#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "plane.h"

class Triangle: public Object3D {

  public:
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *t): v0(a), v1(b), v2(c) {
      type = t;
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
      Plane plane(v0, v1, v2, type);
      Hit h1(h);
      if (plane.intersect(r, h1, tmin)) {
        Vec3f p(h1.getIntersectionPoint());
        Vec3f l0(v0 - p), l1(v1 - p), l2(v2 - p);
        float s0 = l0.Cross3(l1).Length();
        float s1 = l1.Cross3(l2).Length();
        float s2 = l2.Cross3(l0).Length();
        float s  = (v1 - v0).Cross3(v2 - v0).Length();
        if (s0 + s1 + s2 - s <= 1e-6 && s - s0 - s1 - s2 <= 1e-6) {
          h = h1;
          return 1;
        } else return 0;
      } else return 0;
    }

  protected:
    Vec3f v0, v1, v2;

};

#endif