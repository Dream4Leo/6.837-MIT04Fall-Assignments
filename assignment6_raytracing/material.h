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
    Vec3f getDiffuseColor() const { return diffuseColor; }
    virtual Vec3f getSpecularColor() const = 0;
    virtual Vec3f getReflectiveColor() const = 0;
    virtual Vec3f getTransparentColor() const = 0;
    virtual float getIndexOfRefraction() const = 0;
    // MODIFIER
    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
      const Vec3f &lightColor) const = 0;

  protected:
    Vec3f diffuseColor;
  
};

class PhongMaterial: public Material {
  
  public:
    PhongMaterial(const Vec3f &d, const Vec3f &s, float e, 
                  const Vec3f &r, const Vec3f &t, float i): 
      specularColor(s), exponent(e), reflectiveColor(r), 
      transparentColor(t), indexOfRefraction(i) { diffuseColor = d; }
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
    virtual Vec3f getReflectiveColor() const { return reflectiveColor; }
    virtual Vec3f getTransparentColor() const { return transparentColor; }
    virtual float getIndexOfRefraction() const { return indexOfRefraction; }

  protected:
    Vec3f specularColor;
    float exponent;
    Vec3f reflectiveColor;
    Vec3f transparentColor;
    float indexOfRefraction;
};

class Checkerboard: public Material {

  public:
    Checkerboard(Matrix *m, Material *mat1, Material *mat2) {}
    ~Checkerboard() {}

    // ACCESSORS
    virtual Vec3f getSpecularColor() const {}
    virtual Vec3f getReflectiveColor() const {}
    virtual Vec3f getTransparentColor() const {}
    virtual float getIndexOfRefraction() const {}
    // MODIFIER
    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
      const Vec3f &lightColor) const {}
};

class Noise: public Material {
  
  public:
    Noise(Matrix *m, Material *mat1, Material *mat2, int octaves) {}
    ~Noise() {}
    
    // ACCESSORS
    virtual Vec3f getSpecularColor() const {}
    virtual Vec3f getReflectiveColor() const {}
    virtual Vec3f getTransparentColor() const {}
    virtual float getIndexOfRefraction() const {}
    // MODIFIER
    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
      const Vec3f &lightColor) const {}
};

class Wood: public Material {

  public:
    Wood(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) {}
    ~Wood() {}
    
    // ACCESSORS
    virtual Vec3f getSpecularColor() const {}
    virtual Vec3f getReflectiveColor() const {}
    virtual Vec3f getTransparentColor() const {}
    virtual float getIndexOfRefraction() const {}
    // MODIFIER
    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
      const Vec3f &lightColor) const {}
};

class Marble: public Material {

  public:
    Marble(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) {}
    ~Marble() {}
    
    // ACCESSORS
    virtual Vec3f getSpecularColor() const {}
    virtual Vec3f getReflectiveColor() const {}
    virtual Vec3f getTransparentColor() const {}
    virtual float getIndexOfRefraction() const {}
    // MODIFIER
    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
      const Vec3f &lightColor) const {}
};


#endif