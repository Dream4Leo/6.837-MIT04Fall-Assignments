#ifndef _RENDER_H_
#define _RENDER_H_

#include "image.cpp"
#include "scene_parser.cpp"

class Render {

  public:

    Render(SceneParser *_scene): scene(_scene) {}
    ~Render() {}

    void setOutput(char *file, int width, int height) {
      output_file = file;
      output_width = width;
      output_height = height;
    }

    void phong(bool shade_back);
    void grey(float depth_min, float depth_max);
    void normal();

  protected:
    SceneParser* scene;
    char* output_file;
    int output_width;
    int output_height;
};

#endif