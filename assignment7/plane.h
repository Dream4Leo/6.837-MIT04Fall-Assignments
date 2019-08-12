#ifndef _PLANE_H
#define _PLANE_H

#include "object3d.h"
#include "vectors.h"

class Plane: public Object3D {

  public:
    
    Plane(Vec3f &n, float d, Material *t): normal(n), offset(d) {
      type = t;
      bbox = NULL;
      normal.Normalize();
    }
    Plane(Vec3f &v0, Vec3f &v1, Vec3f &v2, Material *t) {
      normal = (v1 - v0).Cross3(v2 - v0);
      normal.Normalize();
      offset = v0.Dot3(normal);
      type = t;
      bbox = NULL;
    }
    ~Plane() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
      RayTracingStats::IncrementNumIntersections();
      float theta = (normal.Dot3(r.getDirection()));
      if (theta == 0.0) return 0;
      float t = -(-offset + normal.Dot3(r.getOrigin())) / theta;
      if (t < h.getT() && t > tmin) {
        h.set(t, type, normal, r);
        return 1;
      } else
        return 0;
    }
    void insertIntoGrid(Grid *g, Matrix *m) { mat = NULL; }

  protected:
    Vec3f normal;
    float offset;

};

#endif