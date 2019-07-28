#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include <math.h>

class Transform: public Object3D {

  public:
    Transform(Matrix &m, Object3D *o): mat(m), obj(o) {
      Vec3f t[2], *b[8];
      t[0] = o->getBBox()->getMin();
      t[1] = o->getBBox()->getMax();
      for (int i = 0; i < 8; ++i) {
        b[i] = new Vec3f(t[!!(i&4)].x(), t[!!(i&2)].y(), t[i&1].z());
        m.Transform(*b[i]);
      }
      bbox = new BoundingBox(*b[0], *b[0]);
      for (int i = 1; i < 8; ++i) bbox->Extend(*b[i]);
      for (int i = 1; i < 8; ++i) delete b[i];
    }
    ~Transform() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
      //Inverse Matrix
      Matrix mat_inv(mat);
      mat_inv.Inverse();

      //Transform origin and direction to object space
      Vec3f origin_os(r.getOrigin());
      mat_inv.Transform(origin_os);
      Vec3f direction_os(r.getDirection());
      mat_inv.TransformDirection(direction_os);

      direction_os.Normalize();
      Ray r_os(origin_os, direction_os);
      Hit h_os(1e8, NULL, Vec3f(0,0,0));

      if (obj->intersect(r_os, h_os, tmin)) {
        Vec3f normal_ws = h_os.getNormal();
        mat_inv.Transpose();
        mat_inv.Transform(normal_ws);

        Vec3f p_ws(h_os.getIntersectionPoint());
        mat.Transform(p_ws);
        float t = (p_ws-r.getOrigin()).Length();
        if (t < h.getT() && t > tmin)
          h.set(t, h_os.getMaterial(), normal_ws, r);

        return 1;
      } else return 0;
    }
    void insertIntoGrid(Grid *g, Matrix *m) {
      Matrix *mo = new Matrix(m == NULL ? mat : *m * mat);
      obj->insertIntoGrid(g, mo);
      delete mo;
    }

  protected:
    Object3D* obj;
    Matrix mat;

};

#endif