#ifndef _RENDER_H_
#define _RENDER_H_

#include <assert.h>

#include "scene_parser.h"
#include "image.h"
#include "grid.h"
#include "ray.h"
#include "hit.h"

class Render {

public:
	Render() {}
	Render(SceneParser *_scene, int _max_bounces, float _cutoff_weight, bool _shadows, bool _shade_back, int nx, int ny, int nz): 
		scene(_scene), max_bounces(_max_bounces), cutoff_weight(_cutoff_weight), shadows(_shadows), shade_back(_shade_back) {
		if (nx != 0) {
			grid = new Grid(scene->getGroup()->getBBox(), nx, ny, nz);
			scene->getGroup()->insertIntoGrid(grid, NULL);
		} else 
			grid = NULL;
	}
	~Render() {}
	//Render Methods
	void phong(	char *file, int width, int height);
	void depth(	char *file, int width, int height, float depth_min, float depth_max);
	void normal(char *file, int width, int height);
	void voxel(	char *file, int width, int height);

protected:
	Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, 
									float indexOfRefraction, Hit &hit) const;
	bool rayCast(Ray &ray, Hit &hit, float tmin) const;
	Vec3f intersectGrid(Ray &ray, float tmin);

	SceneParser* scene;
	int		max_bounces;
	float	cutoff_weight;
	bool	shade_back;
	bool	shadows;

	Grid *grid;
};

void Render::phong(char *file, int width, int height) {
	assert(file != NULL);
	Image image(width, height);
	image.SetAllPixels(scene->getBackgroundColor());
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y) {
			Vec2f point(1.0*x / width, 1.0*y / height);
			Ray r(scene->getCamera()->generateRay(point));
			Hit h(1e8, NULL, Vec3f(0,0,0));
			RayTracingStats::IncrementNumNonShadowRays();
			Vec3f color = traceRay(r, 1e-4, 0, cutoff_weight, 1.0, h);
			color.Clamp();
			image.SetPixel(x, y, color);
		}
	image.SaveTGA(file);
}

void Render::depth(char *file, int width, int height, float depth_min, float depth_max) {
	assert(file != NULL);
	Image image(width, height);
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y) {

			Vec2f point(1.0*x / width, 1.0*y / height);
			Ray r(scene->getCamera()->generateRay(point));
			Hit h(1e8, NULL, Vec3f(0,0,0));
			scene->getGroup()->intersect(r, h, 0);
			float t = (depth_max - h.getT()) / (depth_max - depth_min);
			Vec3f grey(t, t, t); grey.Clamp();
			image.SetPixel(x, y, grey);
		}
	image.SaveTGA(file);
}

void Render::normal(char *file, int width, int height) {
	assert(file != NULL);
	Image image(width, height);
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y) {

			Vec2f point(1.0*x / width, 1.0*y / height);
			Ray r(scene->getCamera()->generateRay(point));
			Hit h(1e8, NULL, Vec3f(0,0,0));
			scene->getGroup()->intersect(r, h, 0);
			Vec3f color = h.getNormal();
			color.Set(fabs(color.x()), fabs(color.y()), fabs(color.z()));
			image.SetPixel(x, y, color);
		}
	image.SaveTGA(file);
}

Vec3f Render::traceRay(Ray &ray, float tmin, int bounces, 
	float weight, float indexOfRefraction, Hit &hit) const {
	
	if (!rayCast(ray, hit, tmin)) return scene->getBackgroundColor();
	
	Vec3f norm = hit.getNormal(); norm.Normalize(); 
	Vec3f p(hit.getIntersectionPoint());
	Material *material = hit.getMaterial();
	assert(material != NULL);

	//ambient
	Vec3f ambient = norm.Dot3(ray.getDirection()) < 0.0 ? scene->getAmbientLight() : Vec3f(0,0,0);
 	
	//diffuse & specular
	Vec3f diffuse(0.0, 0.0, 0.0);
	Vec3f specular(0.0, 0.0, 0.0);
	Vec3f dirToLight, lightcolor;
	float disToLight, c;

	int n = scene->getNumLights();
	for (int i = 0; i < n; ++i) {
		scene->getLight(i)->getIllumination(p, dirToLight, lightcolor, disToLight);
		// cast shadowRay
		if (shadows) {
			Ray shadowRay(p, dirToLight);
			Hit h(1e8, NULL, Vec3f(0,0,0));
			RayTracingStats::IncrementNumShadowRays();
			if (rayCast(shadowRay, h, tmin) && h.getT() < disToLight) continue; // intersection
		}		
		dirToLight.Normalize();
		c = dirToLight.Dot3(norm);
		diffuse += (c < 0.0 ? (shade_back ? -c: 0.0) : c) * lightcolor;
		specular += material->Shade(ray, hit, dirToLight, lightcolor);
	}
	//mix
	Vec3f color = (ambient + diffuse) * material->getDiffuseColor()
												 + specular * material->getSpecularColor();
	//cutoff
	if (bounces == max_bounces) return color;
	// if (color.x() < weight && color.y() < weight && color.z() < weight) return color;
	
	//Recursive RayTracing
	//Reflection
	if (material->getReflectiveColor().Length() > 1e-6) {
		Hit h(1e8, NULL, Vec3f(0,0,0));
		Vec3f V(ray.getDirection()); V.Normalize();
		Vec3f R(V-2*V.Dot3(norm)*norm);
		Ray reflectRay(p, R);
		color += material->getReflectiveColor() * 
						 traceRay(reflectRay, tmin, bounces + 1, weight, indexOfRefraction, h);
	}
	//Refraction
	if (material->getTransparentColor().Length() > 1e-6) {
		float n = indexOfRefraction / material->getIndexOfRefraction();
		Vec3f I(-1.0*ray.getDirection()); I.Normalize();
		float c = norm.Dot3(I);
		if (c < 0.0) n = 1.0 / n, norm = -1.0 * norm, c = -c;	// outwards
		float delta = 1.0-n*n*(1.0-c*c);
		if (delta >= 0.0) {
			Hit h(1e8, NULL, Vec3f(0,0,0));
			Vec3f T((n*c-sqrt(delta))*norm-n*I);
			Ray refractRay(p, T);
			color += material->getTransparentColor() * 
							traceRay(refractRay, tmin, bounces + 1, weight, indexOfRefraction, h);
		}
	}
	return color;
}

void Render::voxel(char *file, int width, int height) {
	assert(file != NULL);
	Image image(width, height);
	image.SetAllPixels(scene->getBackgroundColor());

	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y) {
			Vec2f point(1.0*x / width, 1.0*y / height);
			Ray r(scene->getCamera()->generateRay(point));
			Vec3f color = intersectGrid(r, 1e-8);
			color.Clamp();
			image.SetPixel(x, y, color);
		}
	image.SaveTGA(file);
}

Vec3f Render::intersectGrid(Ray &ray, float tmin) {
	MarchingInfo mi;
	grid->initializeRayMarch(mi, ray, tmin); 
	if (mi.tmin == 0.0) return scene->getBackgroundColor(); // no intersection
	Object3DVector *voxel = grid->getVoxel(mi.i, mi.j, mi.k);
	while (voxel != NULL && voxel->getNumObjects() == 0) voxel = grid->getNextVoxel(mi);
	return voxel == NULL ? scene->getBackgroundColor() : mi.getColor() * mi.hit_normal;
}

bool Render::rayCast(Ray &ray, Hit &hit, float tmin) const{
	if (grid == NULL) return scene->getGroup()->intersect(ray, hit, tmin);
	
	MarchingInfo mi;
	grid->initializeRayMarch(mi, ray, tmin);

	//Handle planes seperately
	Object3DVector *planes = scene->getPlanes();
	int n = planes->getNumObjects();
	for (int i = 0; i < n; ++i) 
		planes->getObject(i)->intersect(ray, hit, tmin);
	if (mi.tmin == 0.0) return hit.getT() < 1e8;
	
	Hit h; h.set(1e8, NULL, Vec3f(0,0,0), ray);
	//Handle other primitives (sphere/triangle)
	Object3DVector *voxel = grid->getVoxel(mi.i, mi.j, mi.k);
	for (;voxel != NULL; voxel = grid->getNextVoxel(mi)) {
		RayTracingStats::IncrementNumGridCellsTraversed();
		int n = voxel->getNumObjects(); if (!n) continue;
		h.set(1e8, NULL, Vec3f(0,0,0), ray);
		for (int i = 0; i < n; ++i) {
			Object3D *obj = voxel->getObject(i);
			Matrix *mat = obj->getMat(); //identity in case of no transform

			//Inverse Matrix
			Matrix mat_inv(*mat);	mat_inv.Inverse();
			//Transform origin and direction to object space
			Vec3f origin_os(ray.getOrigin());
			mat_inv.Transform(origin_os);
			Vec3f direction_os(ray.getDirection());
			mat_inv.TransformDirection(direction_os);
			//Create ray and hit in object space
			direction_os.Normalize();
			Ray r_os(origin_os, direction_os);
			Hit h_os(1e8, NULL, Vec3f(0,0,0));

			if (obj->intersect(r_os, h_os, tmin)) {			
				//Returning to world space
				Vec3f normal_ws = h_os.getNormal();
				mat_inv.Transpose();
				mat_inv.Transform(normal_ws);
				Vec3f p_ws(h_os.getIntersectionPoint());
				mat->Transform(p_ws);
				float t = (p_ws-ray.getOrigin()).Length();
				//intersection within t-range and inside voxel
				if (t < h.getT() && t > tmin && grid->insideVoxel(mi.i, mi.j, mi.k, p_ws))
					h.set(t, h_os.getMaterial(), normal_ws, ray);
			}
		}
		if (h.getT() < 1e8) break;	//intersection found
	}
	if (h.getT() < hit.getT()) hit = h; //combine results with planes
	return hit.getT() < 1e8;
}

#endif