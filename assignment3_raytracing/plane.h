#ifndef _PLANE_H
#define _PLANE_H

#include "object3d.h"
#include "vectors.h"

class Plane: public Object3D {

  public:
    
    Plane(Vec3f &n, float d, Material *t): normal(n), offset(d) {
      type = t;
    }

    Plane(Vec3f &v0, Vec3f &v1, Vec3f &v2, Material *t) {
      normal = (v1 - v0).Cross3(v2 - v0);
      normal.Normalize();
      offset = v0.Dot3(normal);
      type = t;
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
      float t = -(-offset + normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
      if (t < tmin) return 0;
      if (t < h.getT()) h.set(t, type, normal,r);
      return 1;
    }

  protected:
    Vec3f normal;
    float offset;

};

#endif