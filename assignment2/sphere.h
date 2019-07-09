#ifndef _SPHERE_H
#define _SPHERE_H


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
      if (pos.Length() > radius && tp < 0) return 0; //outside and behind
      float d2 = pos.Dot3(pos) - tp * tp;
      float r2 = radius * radius;
      if (d2 > r2) return 0; //shot miss
      float t;
      if (pos.Length() > radius) t = tp - sqrt(r2-d2);
      if (pos.Length() < radius) t = tp + sqrt(r2-d2);
      Vec3f normal = r.pointAtParameter(t) - center;
      normal.Normalize();
      if (t < h.getT()) h.set(t, type, normal, r);
      return 1;
    }

  protected:
    Vec3f center;
    float radius;

};


#endif