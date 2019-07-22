#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "scene_parser.h"
#include "render.h"

char *input_file    = NULL;
char *output_file   = NULL;
char *depth_file    = NULL;
char *normal_file   = NULL;
int   width         = 100;
int   height        = 100;
float depth_min     = 0;
float depth_max     = 1;
int   max_bounces   = 0;
float cutoff_weight = 100;
bool  shadows       = 0;
bool  shade_back    = 0;

void parser(int argc, char *argv[]);

int main(int argc, char *argv[]) {

  parser(argc, argv);
  SceneParser scene(input_file);
  Render r(&scene, max_bounces, cutoff_weight, shadows, shade_back);

  if (output_file != NULL) r.phong( output_file, width, height);
  if (depth_file  != NULL) r.depth(  depth_file,  width, height, depth_min, depth_max);
  if (normal_file != NULL) r.normal(normal_file, width, height);
	
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
    } else {
      printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
      assert(0);
    }
  }
}