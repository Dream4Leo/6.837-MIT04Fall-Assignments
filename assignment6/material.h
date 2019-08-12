#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "perlin_noise.h"
#include "vectors.h"
#include "ray.h"
#include "hit.h"

#ifdef SPECULAR_FIX
extern int SPECULAR_FIX_WHICH_PASS;
#endif

class Material {

public:
  Material() {}
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  virtual ~Material() {}

  virtual Vec3f getDiffuseColor(Vec3f p) const = 0;
  virtual Vec3f getSpecularColor(Vec3f p) const = 0;
  virtual Vec3f getReflectiveColor(Vec3f p) const = 0;
  virtual Vec3f getTransparentColor(Vec3f p) const = 0;
  virtual float getIndexOfRefraction(Vec3f p) const = 0;
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
    float co = n.Dot3(mid), fix = n.Dot3(dirToLight);
    if ( fix < 0 ) return Vec3f(0.0, 0.0, 0.0);
    return fix * (co < 0.0 ? 0.0 : pow(co, exponent)) * lightColor;
  }
  virtual Vec3f getDiffuseColor(Vec3f p) const { return diffuseColor; }
  virtual Vec3f getSpecularColor(Vec3f p) const { return specularColor; }
  virtual Vec3f getReflectiveColor(Vec3f p) const { return reflectiveColor; }
  virtual Vec3f getTransparentColor(Vec3f p) const { return transparentColor; }
  virtual float getIndexOfRefraction(Vec3f p) const { return indexOfRefraction; }

protected:
  Vec3f specularColor;
  float exponent;
  Vec3f reflectiveColor;
  Vec3f transparentColor;
  float indexOfRefraction;
};

class Checkerboard: public Material {

public:
  Checkerboard(Matrix *m, Material *mat1, Material *mat2):
    mat(m) {material[0] = mat1; material[1] = mat2;}
  ~Checkerboard() {}

  virtual Vec3f getDiffuseColor(Vec3f p) const {
    mat->Transform(p);
    int x = int(floor(p.x()) + floor(p.y()) + floor(p.z())) & 1;
    return material[x]->getDiffuseColor(p);
  }
  virtual Vec3f getSpecularColor(Vec3f p) const {
    mat->Transform(p);
    int x = int(floor(p.x()) + floor(p.y()) + floor(p.z())) & 1;
    return material[x]->getSpecularColor(p);
  }
  virtual Vec3f getReflectiveColor(Vec3f p) const {
    mat->Transform(p);
    int x = int(floor(p.x()) + floor(p.y()) + floor(p.z())) & 1;
    return material[x]->getReflectiveColor(p);
  }
  virtual Vec3f getTransparentColor(Vec3f p) const {
    mat->Transform(p);
    int x = int(floor(p.x()) + floor(p.y()) + floor(p.z())) & 1;
    return material[x]->getTransparentColor(p);
  }
  virtual float getIndexOfRefraction(Vec3f p) const {
    mat->Transform(p);
    int x = int(floor(p.x()) + floor(p.y()) + floor(p.z())) & 1;
    return material[x]->getIndexOfRefraction(p);
  }
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, 
    const Vec3f &dirToLight, const Vec3f &lightColor) const {
      Vec3f p = hit.getIntersectionPoint(); mat->Transform(p);
      int x = int(floor(p.x()) + floor(p.y()) + floor(p.z())) & 1;
      return material[x]->Shade(ray, hit, dirToLight, lightColor);
    }

protected:
  Material *material[2];
  Matrix *mat;
};

class Noise: public Material {
  
public:
  Noise(Matrix *m, Material *mat1, Material *mat2, int oct):
    mat(m), octaves(oct) {material[0] = mat1; material[1] = mat2;}
  ~Noise() {}
  
  virtual Vec3f getDiffuseColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getDiffuseColor(p)
     + (1-x) * material[1]->getDiffuseColor(p);
  }
  virtual Vec3f getSpecularColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getSpecularColor(p)
     + (1-x) * material[1]->getSpecularColor(p);
  }
  virtual Vec3f getReflectiveColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getReflectiveColor(p)
     + (1-x) * material[1]->getReflectiveColor(p);
  }
  virtual Vec3f getTransparentColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getTransparentColor(p)
     + (1-x) * material[1]->getTransparentColor(p);
  }
  virtual float getIndexOfRefraction(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getIndexOfRefraction(p)
     + (1-x) * material[1]->getIndexOfRefraction(p);
  }
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const {
    Vec3f p = hit.getIntersectionPoint(); mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->Shade(ray, hit, dirToLight, lightColor)
     + (1-x) * material[1]->Shade(ray, hit, dirToLight, lightColor);
  }
  
protected:
  float N(float x, float y, float z) const {
    float n = 0.0;
    for (int i = 0, j = 1; i < octaves; ++i, j <<= 1) 
      n += PerlinNoise::noise(x*j, y*j, z*j) / j;
    return n;
  }

  Material *material[2];
  Matrix *mat;
  int octaves;
};

class Marble: public Material {

public:
  Marble(Matrix *m, Material *mat1, Material *mat2, int oct, float freq, float amp):
    mat(m), octaves(oct), frequency(freq), amplitude(amp) {material[0] = mat1; material[1] = mat2;}
  ~Marble() {}
  
  virtual Vec3f getDiffuseColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getDiffuseColor(p)
     + (1-x) * material[1]->getDiffuseColor(p);
  }
  virtual Vec3f getSpecularColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getSpecularColor(p)
     + (1-x) * material[1]->getSpecularColor(p);
  }
  virtual Vec3f getReflectiveColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getReflectiveColor(p)
     + (1-x) * material[1]->getReflectiveColor(p);
  }
  virtual Vec3f getTransparentColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getTransparentColor(p)
     + (1-x) * material[1]->getTransparentColor(p);
  }
  virtual float getIndexOfRefraction(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getIndexOfRefraction(p)
     + (1-x) * material[1]->getIndexOfRefraction(p);
  }
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const {
    Vec3f p = hit.getIntersectionPoint(); mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->Shade(ray, hit, dirToLight, lightColor)
     + (1-x) * material[1]->Shade(ray, hit, dirToLight, lightColor);
  }
protected:

  float N(float x, float y, float z) const {
    float n = 0.0;
    for (int i = 0, j = 1; i < octaves; ++i, j <<= 1) 
      n += PerlinNoise::noise(x*j, y*j, z*j) / j;
    // n = n > 1.0 ? 1.0 : (n < 0.0 ? 0.0 : n);
    return sin(frequency*x + amplitude*n);
  }

  Material *material[2];
  Matrix *mat;
  int octaves;
  float frequency;
  float amplitude;
};

class Wood: public Material {

public:
  Wood(Matrix *m, Material *mat1, Material *mat2, int oct, float freq, float amp):
    mat(m), octaves(oct), frequency(freq), amplitude(amp) {material[0] = mat1; material[1] = mat2;}
  ~Wood() {}
  
  virtual Vec3f getDiffuseColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getDiffuseColor(p)
     + (1-x) * material[1]->getDiffuseColor(p);
  }
  virtual Vec3f getSpecularColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getSpecularColor(p)
     + (1-x) * material[1]->getSpecularColor(p);
  }
  virtual Vec3f getReflectiveColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getReflectiveColor(p)
     + (1-x) * material[1]->getReflectiveColor(p);
  }
  virtual Vec3f getTransparentColor(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getTransparentColor(p)
     + (1-x) * material[1]->getTransparentColor(p);
  }
  virtual float getIndexOfRefraction(Vec3f p) const {
    mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->getIndexOfRefraction(p)
     + (1-x) * material[1]->getIndexOfRefraction(p);
  }
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const {
    Vec3f p = hit.getIntersectionPoint(); mat->Transform(p);
    float x = N(p.x(), p.y(), p.z());
    return x * material[0]->Shade(ray, hit, dirToLight, lightColor)
     + (1-x) * material[1]->Shade(ray, hit, dirToLight, lightColor);
  }
protected:

  float N(float x, float y, float z) const {
    float n = 0.0;
    for (int i = 0, j = 1; i < octaves; ++i, j <<= 1) 
      n += PerlinNoise::noise(x*j, y*j, z*j) / j;
    // n = n > 1.0 ? 1.0 : (n < 0.0 ? 0.0 : n);
    return pow(cos(2*(x*x+z*z) + 0.1*frequency*n + 0.1*amplitude*y), 2);
  }

  Material *material[2];
  Matrix *mat;
  int octaves;
  float frequency;
  float amplitude;
};


#endif