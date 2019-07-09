#include "render.h"

void Render::phong(bool shade_back) {
  assert(output_file != NULL);
  Image image(output_width, output_height);
  image.SetAllPixels(scene->getBackgroundColor());

  for (int x = 0; x < output_width; ++x)
    for (int y = 0; y < output_height; ++y) {

      Vec2f point(1.0*x/output_width, 1.0*y/output_height);
      Ray r = scene->getCamera()->generateRay(point);
      Vec3f normal(0.0, 0.0, 0.0);
      Hit h(1e8, NULL, normal);

      if (scene->getGroup()->intersect(r, h, 0)) {

        Vec3f color = h.getMaterial()->getDiffuseColor();
        //ambient
        Vec3f ambient = scene->getAmbientLight();
        //diffuse
        Vec3f diffuse(0.0, 0.0, 0.0);
        Vec3f dirlight, clight;
        int n = scene->getNumLights();
        for (int i = 0; i < n; ++i) {
          Vec3f normal = h.getNormal();
          scene->getLight(i)->getIllumination(
            h.getIntersectionPoint(),dirlight,clight);
          float co = dirlight.Dot3(normal) / dirlight.Length() / normal.Length();
          diffuse += (co < 0.0 ? (shade_back ? -co : 0) : co) * clight;
        }
        //specular to be added
        
        //mix
        color = color * (ambient + diffuse);
        color.Clamp();

        image.SetPixel(x, y, color);
      }
    }
  image.SaveTGA(output_file);
}

void Render::grey(float depth_min, float depth_max) {
  assert(output_file != NULL);
  Image image(output_width, output_height);
  for (int x = 0; x < output_width; ++x)
    for (int y = 0; y < output_height; ++y) {

      Vec2f point(1.0*x/output_width, 1.0*y/output_height);
      Ray r = scene->getCamera()->generateRay(point);
      Vec3f normal(0.0, 0.0, 0.0);
      Hit h(1e8, NULL, normal);
      scene->getGroup()->intersect(r, h, 0);
      float t = (depth_max - h.getT()) / (depth_max - depth_min);
      Vec3f grey(t, t, t);
      grey.Clamp();
      image.SetPixel(x, y, grey);
    }
  image.SaveTGA(output_file);
}

void Render::normal() {
  assert(output_file != NULL);
  Image image(output_width, output_height);
  for (int x = 0; x < output_width; ++x)
    for (int y = 0; y < output_height; ++y) {

      Vec2f point(1.0*x/output_width, 1.0*y/output_height);
      Ray r = scene->getCamera()->generateRay(point);
      Vec3f normal(0.0, 0.0, 0.0);
      Hit h(1e8, NULL, normal);
      scene->getGroup()->intersect(r, h, 0);
      Vec3f color = h.getNormal();
      float cx, cy, cz;
      if (color.x() < 0.0) cx = -color.x(); else cx = color.x();
      if (color.y() < 0.0) cy = -color.y(); else cy = color.y();
      if (color.z() < 0.0) cz = -color.z(); else cz = color.z();
      color.Set(cx, cy, cz);
      image.SetPixel(x, y, color);
    }
  image.SaveTGA(output_file);
}