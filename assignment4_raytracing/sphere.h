#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "object3d.h"
#include "vectors.h"

const float PI = 3.1415926;

class Sphere: public Object3D {

  public:
    Sphere (Vec3f c, float r, Material *t):center(c), radius(r) {
      type = t;
    }
    ~Sphere () {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
      Vec3f pos = r.getOrigin() - center; 
      Vec3f dir = r.getDirection();
      
      float tp = -dir.Dot3(pos);
      if (pos.Length() > radius && tp < 0.0) return 0; //outside and behind
      float d2 = pos.Dot3(pos) - tp * tp;
      float r2 = radius * radius;
      if (d2 > r2) return 0; //shot miss
      float t;
      if (pos.Length() > radius - 1e-4) t = tp - sqrt(r2-d2);
      if (pos.Length() < radius + 1e-4) t = tp + sqrt(r2-d2);
      Vec3f normal = r.pointAtParameter(t) - center;
      normal.Normalize();
      if (t < h.getT() && t > tmin) {
        h.set(t, type, normal, r);
        return 1;
      } else
        return 0;
    }
    
  protected:
    Vec3f center;
    float radius;

};

#endif