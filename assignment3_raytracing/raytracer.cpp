#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "render.h"


char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char *depth_file = NULL;
char *normal_file = NULL;
bool shade_back = 0;

void parser(int argc, char *argv[]);

Render *ptr;

int main(int argc, char *argv[]) {

  parser(argc, argv);
  SceneParser scene(input_file);
  Render r(&scene);

	ptr = &r;

	if (shade_back)
		r.setBack();

  if (output_file != NULL) {
    r.setOutput(output_file, width, height);
    r.phong();
  }

  if (depth_file != NULL) {
    r.setOutput(depth_file, width, height);
    r.grey(depth_min, depth_max);
  }

  if (normal_file != NULL) {
    r.setOutput(normal_file, width, height);
    r.normal();
  }
	
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
    } else {
      printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
      assert(0);
    }
  }
}