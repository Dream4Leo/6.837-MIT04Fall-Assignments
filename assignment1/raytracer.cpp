
#include "camera.h"
#include "image.cpp"
#include "object3d.h"
#include "scene_parser.cpp"

int main(int argc, char *argv[]) {

  char *input_file = NULL;
  int width = 100;
  int height = 100;
  char *output_file = NULL;
  float depth_min = 0;
  float depth_max = 1;
  char *depth_file = NULL;

  // sample command line:
  // raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

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
    } else {
      printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
      assert(0);
    }
  }

  SceneParser scene(input_file);
  Group* group = scene.getGroup();
  Camera* cam = scene.getCamera();
  Image image0(width, height);
  Image image1(width, height);
  image0.SetAllPixels(scene.getBackgroundColor());

  for (int x = 0; x < width; ++x)
    for (int y = 0; y < height; ++y)
    {
      Vec2f point(1.0*x/width, 1.0*y/height);
      Ray r = cam->generateRay(point);
      Hit h(1e8, NULL);
      
      if ( group->intersect(r, h, -1e8) ) 
        image0.SetPixel(x,y,h.getMaterial()->getDiffuseColor());
      float t = (depth_max - h.getT()) / (depth_max - depth_min);
      Vec3f grey(t,t,t);
      grey.Clamp();
      //grey.Clamp(depth_min,depth_max);
      image1.SetPixel(x,y,grey);
    }
  
  image0.SaveTGA(output_file);
  image1.SaveTGA(depth_file);
  return 0;
}