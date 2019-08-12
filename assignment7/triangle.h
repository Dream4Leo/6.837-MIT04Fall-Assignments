#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include <math.h>

#include "object3d.h"
#include "plane.h"
#include "vectors.h"

float Min(float a, float b, float c) { return min(min(a,b),c); }
float Max(float a, float b, float c) { return max(max(a,b),c); }

class Triangle: public Object3D {

public:
Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *t): v0(a), v1(b), v2(c) {
  type = t;
  bbox = new BoundingBox(a, a);
  bbox->Extend(b); bbox->Extend(c);
}
~Triangle() {}

virtual bool intersect(const Ray &r, Hit &h, float tmin) {
  RayTracingStats::IncrementNumIntersections();
  
  Plane plane(v0, v1, v2, type);
  Hit hplane(h);
  if (plane.intersect(r, hplane, tmin)) {
    Vec3f p(hplane.getIntersectionPoint());
    Vec3f l0(v0 - p), l1(v1 - p), l2(v2 - p);
    float s0 = l0.Cross3(l1).Length();
    float s1 = l1.Cross3(l2).Length();
    float s2 = l2.Cross3(l0).Length();
    float s  = (v1 - v0).Cross3(v2 - v0).Length();
    if (fabs(s0 + s1 + s2 - s) / s <= 1e-6) {
      h = hplane;
      return 1;
    }
  }
  return 0;
}
void insertIntoGrid(Grid *g, Matrix *m) {
  if (m == NULL) {
    mat = new Matrix;
    mat->SetToIdentity();
  } else
    mat = m;
  Vec3f min(getBBox()->getMin()), max(getBBox()->getMax());
  if (m != NULL) {
    Vec3f v[3]; v[0] = v0; v[1] = v1; v[2] = v2;
    for (int i = 0; i < 3; ++i) m->Transform(v[i]);
    BoundingBox b(v[0], v[0]); b.Extend(v[1]); b.Extend(v[2]);
    min = b.getMin(); max = b.getMax();
  }

  Vec3f l, r;
  g->getVoxelID(min, max, l, r);
  for (int i = l.x(); i <= r.x(); ++i)
    for (int j = l.y(); j <= r.y(); ++j)
      for (int k = l.z(); k <= r.z(); ++k)
          g->getVoxel(i, j, k)->addObject(this);
}

protected:
  Vec3f v0, v1, v2;
};

#endif