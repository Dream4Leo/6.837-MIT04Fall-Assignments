#ifndef _RAYTRACING_STATS_H_
#define _RAYTRACING_STATS_H_

#include <time.h>
#include <limits.h>
#include <stdio.h>

#include "vectors.h"
#include "boundingbox.h"

// ====================================================================
// ====================================================================

// This class only contains static variables and static member
// functions so there is no need to call the constructor, destructor
// etc.  It's just a wrapper for the global statistics data.

class RayTracingStats {

public:

  // To be called before computation begins
  static void Initialize(int _width, int _height, BoundingBox *_bbox, 
			 int nx, int ny, int nz);

  // Call for each non-shadow ray
  static void IncrementNumNonShadowRays() { num_nonshadow_rays++; }

  // Call for each shadow ray
  static void IncrementNumShadowRays() { num_shadow_rays++; }

  // Add this to each Object3D primitive's intersect routine 
  // (but not group and transform). 
  // This is a count of the number of times ray-primitive intersection
  // is computed, not the number of times it returns true
  static void IncrementNumIntersections() {    
    // WARNING:  this might overflow
    num_intersections++; }

  // Call each time a new cell is entered by a ray
  static void IncrementNumGridCellsTraversed() { num_grid_cells_traversed++; }

  // Call when you're all done
  static void PrintStatistics();

private:

  // TIMING VARIABLES
  // use long longs to try to avoid overflow problems, but with really
  // big, complex scenes we will still have problems.
  static int width;
  static int height;
  static BoundingBox *bbox;
  static int num_x;
  static int num_y;
  static int num_z;
  static unsigned long long start_time;
  static unsigned long long num_nonshadow_rays;
  static unsigned long long num_shadow_rays;
  static unsigned long long num_intersections;
  static unsigned long long num_grid_cells_traversed;  
};

// ====================================================================
// ====================================================================


int RayTracingStats::width;
int RayTracingStats::height;
BoundingBox* RayTracingStats::bbox;
int RayTracingStats::num_x;
int RayTracingStats::num_y;
int RayTracingStats::num_z;

unsigned long long RayTracingStats::start_time;
unsigned long long RayTracingStats::num_nonshadow_rays;
unsigned long long RayTracingStats::num_shadow_rays;
unsigned long long RayTracingStats::num_intersections;
unsigned long long RayTracingStats::num_grid_cells_traversed;

// ====================================================================
// ====================================================================

void RayTracingStats::Initialize(int _width, int _height, BoundingBox *_bbox, 
				 int nx, int ny, int nz) {
  width = _width;
  height = _height;
  bbox = _bbox;
  num_x = nx;
  num_y = ny;
  num_z = nz;
  start_time = time(NULL);
  num_nonshadow_rays = 0;
  num_shadow_rays = 0;
  num_intersections = 0;
  num_grid_cells_traversed = 0;
}

// ====================================================================
// ====================================================================

void RayTracingStats::PrintStatistics()  {
  
  int delta_time = time(NULL) - start_time;
  if (delta_time == 0) delta_time = 1;
  int secs  = delta_time % 60;
  int min   = (delta_time / 60) % 60; 
  int hours = delta_time / (60*60);
  int num_rays = num_nonshadow_rays + num_shadow_rays;
  float rays_per_sec =  float(num_rays) / float(delta_time);
  float rays_per_pixel =  float(num_rays) / float(width*height);
  float intersections_per_ray = num_intersections / float(num_rays);
  float traversed_per_ray = num_grid_cells_traversed / float(num_rays);

  printf ("********************************************\n");
  printf ("RAY TRACING STATISTICS\n");
  printf ("  total time                 %ld:%02d:%02d\n",hours,min,secs);
  printf ("  num pixels                 %d (%dx%d)\n", width*height,width,height);
  printf ("  scene bounds               ");
  if (bbox == NULL) printf ("NULL\n"); 
  else bbox->Print();
  printf ("  num grid cells             ");
  if (num_x == 0) printf ("NULL\n"); 
  else printf ("%d (%dx%dx%d)\n", num_x*num_y*num_z,num_x,num_y,num_z);
  printf ("  num non-shadow rays        %lld\n", num_nonshadow_rays);
  printf ("  num shadow rays            %lld\n", num_shadow_rays);
  printf ("  total intersections        %lld\n", num_intersections);
  printf ("  total cells traversed      %lld\n", num_grid_cells_traversed);
  printf ("  rays per second            %0.1f\n",rays_per_sec);
  printf ("  rays per pixel             %0.1f\n",rays_per_pixel);
  printf ("  intersections per ray      %0.1f\n",intersections_per_ray);
  printf ("  cells traversed per ray    %0.1f\n",traversed_per_ray);
  printf ("********************************************\n");
  
}

// ====================================================================
// ====================================================================



#endif
