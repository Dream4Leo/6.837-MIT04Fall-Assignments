#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "ray.h"
#include "hit.h"

#ifdef SPECULAR_FIX
extern int SPECULAR_FIX_WHICH_PASS;
#endif

class Material {

public:
  // CONSTRUCTORS & DESTRUCTOR
  Material() {}
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  virtual ~Material() {}

  // ACCESSORS
  virtual Vec3f getDiffuseColor() const { return diffuseColor; }
  virtual Vec3f getSpecularColor() const = 0;
  // MODIFIER
  virtual Vec3f Shade
    (const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
     const Vec3f &lightColor) const = 0;

protected:
  Vec3f diffuseColor;
  
};

class PhongMaterial: public Material {
  
  public:
    PhongMaterial(const Vec3f &d, const Vec3f &s, float e): 
      specularColor(s), exponent(e) { diffuseColor = d; }
    ~PhongMaterial() {}

    virtual Vec3f Shade(const Ray &ray, const Hit &hit, 
      const Vec3f &dirToLight, const Vec3f &lightColor) const {
      
      Vec3f mid = dirToLight - ray.getDirection(); mid.Normalize();
      Vec3f n = hit.getNormal(); n.Normalize();
      float co = n.Dot3(mid);
      float fix = n.Dot3(dirToLight);
      if ( fix < 0 ) return Vec3f(0.0, 0.0, 0.0);
      return fix * (co < 0.0 ? 0.0 : pow(co, exponent)) * lightColor;
    }

    virtual Vec3f getSpecularColor() const { return specularColor; }

  protected:
    Vec3f specularColor;
    float exponent;
};

#endif