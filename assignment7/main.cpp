#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

char *input_file    = NULL;
char *output_file   = NULL;
char *depth_file    = NULL;
char *normal_file   = NULL;
char *samples_file  = NULL;
char *filter_file   = NULL;
int   width         = 100;
int   height        = 100;
int   max_bounces   = 0;
int   nx            = 0;
int   ny            = 0;
int   nz            = 0;
int   num_samples   = 1;
int   sample_zoom   = 0;
int   filter_zoom   = 0;
bool  shadows       = 0;
bool  shade_back    = 0;
bool  grid_acceler  = 0;
bool  visualize_grid= 0;
bool  stats         = 0;
bool  ramdom_samples= 0;
bool  render_samples= 0;
float depth_min     = 0;
float depth_max     = 1;
float cutoff_weight = 1;
float radius        = 0;
float sigma         = 0;

#include "raytracing_stats.h"
#include "scene_parser.h"
#include "sampler.h"
#include "film.h"

Sampler *sampler    = NULL;
Filter  *filter     = NULL;
Film    *film       = NULL;

#include "render.h"

void parser(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  srand(time(0));

  parser(argc, argv);
  film = new Film(width, height, num_samples);

  SceneParser* scene = new SceneParser(input_file);
  Render* render = new Render(scene);

  if (width != height) scene->getCamera()->fixRatio(1.0*width/height);
  if (grid_acceler) render->setGrid();
  if (sampler == NULL) sampler = new UniformSampler(1);
  if (filter == NULL) filter = new BoxFilter(0.5);

  RayTracingStats::Initialize(width, height, 
    scene->getGroup()->getBBox(), nx, ny, nz);

  if (output_file != NULL || samples_file != NULL || filter_file != NULL)
       if (visualize_grid) render->voxel();
                      else render->phong();
  if (depth_file  != NULL) render->depth();
  if (normal_file != NULL) render->normal();
  
	if (stats) RayTracingStats::PrintStatistics();
  return 0;
}

void parser(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i],"-input")) {
      i++; assert (i < argc); 
      input_file = argv[i];
    } else if (!strcmp(argv[i],"-size")) {
      i++; assert (i < argc); 
      width = atoi(argv[i]);
      i++; assert (i < argc); 
      height = atoi(argv[i]);
    } else if (!strcmp(argv[i],"-output")) {
      i++; assert (i < argc); 
      output_file = argv[i];
    } else if (!strcmp(argv[i],"-depth")) {
      i++; assert (i < argc); 
      depth_min = atof(argv[i]);
      i++; assert (i < argc); 
      depth_max = atof(argv[i]);
      i++; assert (i < argc); 
      depth_file = argv[i];
    } else if (!strcmp(argv[i],"-normals")) {
      i++; assert (i < argc);
      normal_file = argv[i];
		}	else if (!strcmp(argv[i], "-shade_back")) {
			shade_back = 1;
    } else if (!strcmp(argv[i], "-shadows")) {
      shadows = 1;
    } else if (!strcmp(argv[i], "-bounces")) {
      i++; assert(i < argc);
      max_bounces = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-weight")) {
      i++; assert(i < argc);
      cutoff_weight = atof(argv[i]);
    } else if (!strcmp(argv[i], "-grid")) {
      grid_acceler = 1;
      i++; assert(i < argc);
      nx = atoi(argv[i]);
      i++; assert(i < argc);
      ny = atoi(argv[i]);
      i++; assert(i < argc);
      nz = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-visualize_grid")) {
      visualize_grid = 1;
    } else if (!strcmp(argv[i], "-stats")) {
      stats = 1;
    } else if (!strcmp(argv[i], "-render_samples")) {
      i++; assert(i < argc);
      samples_file = argv[i];
      i++; assert(i < argc);
      sample_zoom = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-render_filter")) {
      i++; assert(i < argc);
      filter_file = argv[i];
      i++; assert(i < argc);
      filter_zoom = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-random_samples")) {
      i++; assert(i < argc);
      num_samples = atoi(argv[i]);
      sampler = new RandomSampler(num_samples);
    } else if (!strcmp(argv[i], "-uniform_samples")) {
      i++; assert(i < argc);
      num_samples = atoi(argv[i]);
      sampler = new UniformSampler(num_samples);
    } else if (!strcmp(argv[i], "-jittered_samples")) {
      i++; assert(i < argc);
      num_samples = atoi(argv[i]);
      sampler = new JitteredSampler(num_samples);
    } else if (!strcmp(argv[i], "-box_filter")) {
      i++; assert(i < argc);
      radius = atof(argv[i]);
      filter = new BoxFilter(radius);
    } else if (!strcmp(argv[i], "-tent_filter")) {
      i++; assert(i < argc);
      radius = atof(argv[i]);
      filter = new TentFilter(radius);
    } else if (!strcmp(argv[i], "-gaussian_filter")) {
      i++; assert(i < argc);
      sigma = atof(argv[i]);
      filter = new GaussianFilter(sigma);
    } else {
      printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
      assert(0);
    }
  }
}