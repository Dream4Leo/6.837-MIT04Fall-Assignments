#ifndef _TRANSFORM_H
#define _TRANSFORM_H

class Transform: public Object3D {

  public:
    Transform(Matrix &m, Object3D *o): mat(m), obj(o) {}
    ~Transform() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
      //Inverse Matrix
      Matrix mat_inv(mat);
      mat_inv.Inverse();

      //Transform origin and direction to object space
      Vec3f origin_os(r.getOrigin());
      mat_inv.Transform(origin_os);

      Vec4f d4(r.getDirection(), 0.0);
      mat_inv.Transform(d4);
      Vec3f direction_os(d4.x(),d4.y(),d4.z());
      direction_os.Normalize();
      Ray r_os(origin_os, direction_os);
      Hit h_os(1e8, NULL, direction_os/*anythinig*/);

      if (obj->intersect(r_os, h_os, tmin)) {
        Vec3f normal_ws = h_os.getNormal();
        mat_inv.Transpose();
        mat_inv.Transform(normal_ws);

        Vec3f p_ws(h_os.getIntersectionPoint());
        mat.Transform(p_ws);
        float t = (p_ws-r.getOrigin()).Length();
        if ( t < h.getT() )
          h.set(t, h_os.getMaterial(), normal_ws, r);

        return 1;
      } else return 0;
    }

  protected:
    Object3D* obj;
    Matrix mat;

};

#endif