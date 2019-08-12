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
  Vec3f min(c.x()-r, c.y()-r, c.z()-r);
  Vec3f max(c.x()+r, c.y()+r, c.z()+r);
  bbox = new BoundingBox(min, max);
}
~Sphere () {}

virtual bool intersect(const Ray &r, Hit &h, float tmin) {
  RayTracingStats::IncrementNumIntersections();
  
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
  
void insertIntoGrid(Grid *g, Matrix *m) {
  if (m == NULL) {
    mat = new Matrix;
    mat->SetToIdentity();
  } else
    mat = m;
    
  Vec3f min(getBBox()->getMin()), max(getBBox()->getMax());
  Vec3f r_ws(radius, radius, radius);
  Vec3f c_ws(center);
  if (m != NULL) {
    m->Transform(c_ws);
    m->TransformDirection(r_ws);
    m->Transform(min), m->Transform(max);
    BoundingBox b(min, min); b.Extend(max);
    min = b.getMin(); max = b.getMax();
  }
  Vec3f l, r;
  g->getVoxelID(min, max, l, r);
  for (int i = l.x(); i <= r.x(); ++i)
    for (int j = l.y(); j <= r.y(); ++j)
      for (int k = l.z(); k <= r.z(); ++k) {
        Vec3f p = g->getVoxelPos(i, j, k);
        if (pow((fabs(p.x() - c_ws.x())-g->getVx()/2)/r_ws.x(), 2) + 
            pow((fabs(p.y() - c_ws.y())-g->getVy()/2)/r_ws.y(), 2) +
            pow((fabs(p.z() - c_ws.z())-g->getVz()/2)/r_ws.z(), 2) <= 1.0)
          g->getVoxel(i, j, k)->addObject(this);
      }
}

protected:
  Vec3f center;
  float radius;
};

#endif