#ifndef _RENDER_H_
#define _RENDER_H_

#include "image.h"
#include "scene_parser.h"
#include "ray.h"
#include "hit.h"

class Render {

public:

	Render() {}
	Render(SceneParser *_scene) : scene(_scene) {}
	~Render() {}

	void setBack() { shade_back = 1; }
	void setOutput(char *file, int width, int height) {
		output_file = file;
		output_width = width;
		output_height = height;
	}

	void phong() {
		assert(output_file != NULL);
		Image image(output_width, output_height);
		image.SetAllPixels(scene->getBackgroundColor());

		for (int x = 0; x < output_width; ++x)
			for (int y = 0; y < output_height; ++y) {

				Vec2f point(1.0*x / output_width, 1.0*y / output_height);
				Ray r = scene->getCamera()->generateRay(point);
				Vec3f normal(0.0, 0.0, 0.0);
				Hit h(1e8, NULL, normal);

				if (scene->getGroup()->intersect(r, h, 0)) {
					//ambient
					Vec3f ambient = scene->getAmbientLight();
					//diffuse & specular
					Vec3f diffuse(0.0, 0.0, 0.0);
					Vec3f specular(0.0, 0.0, 0.0);
					Vec3f dirToLight, lightcolor;
					float disToLight = 1.0;
					float co = 0.0;
					int n = scene->getNumLights();
					for (int i = 0; i < n; ++i) {
						Vec3f n = h.getNormal(); n.Normalize();
						scene->getLight(i)->getIllumination(
							h.getIntersectionPoint(), dirToLight, lightcolor, disToLight);
						dirToLight.Normalize();
						co = dirToLight.Dot3(n);
						diffuse += (co < 0.0 ? (shade_back ? 0.0: -co) : co) * lightcolor;
						specular += h.getMaterial()->Shade(r, h, dirToLight, lightcolor);
					}
					//mix
					Vec3f color =  (ambient + diffuse) * h.getMaterial()->getDiffuseColor()
				                          + specular * h.getMaterial()->getSpecularColor();
					color.Clamp();
					image.SetPixel(x, y, color);
				}
			}
		image.SaveTGA(output_file);
	}

	void grey(float depth_min, float depth_max) {
		assert(output_file != NULL);
		Image image(output_width, output_height);
		for (int x = 0; x < output_width; ++x)
			for (int y = 0; y < output_height; ++y) {

				Vec2f point(1.0*x / output_width, 1.0*y / output_height);
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

	void normal() {
		assert(output_file != NULL);
		Image image(output_width, output_height);
		for (int x = 0; x < output_width; ++x)
			for (int y = 0; y < output_height; ++y) {

				Vec2f point(1.0*x / output_width, 1.0*y / output_height);
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

protected:
	SceneParser* scene;
	char* output_file;
	int output_width;
	int output_height;
	bool shade_back;
};

#endif