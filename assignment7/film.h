#ifndef _FILM_H_
#define _FILM_H_

#include <assert.h>
#include "sample.h"
#include "image.h"
#include "math.h"

class Filter;

// ==================================================================
// A helper class for super-sampling.  Stores a set of samples for
// each pixel of the final image.

class Film {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Film(int _width, int _height, int _num_samples) { 
    width = _width;
    height = _height;
    num_samples = _num_samples;
    assert(width > 0 && height > 0 && num_samples > 0);
    samples = new Sample[width*height*num_samples]; }
  ~Film() { delete [] samples; }

  // ACCESSORS
  int getWidth() { return width; }
  int getHeight() { return height; }
  int getNumSamples() { return num_samples; }
  Sample getSample(int i, int j, int n) {
    return samples[getIndex(i,j,n)]; }

  // MODIFIERS
  void setSample(int x, int y, int i, Vec2f offset, Vec3f color) {
    samples[getIndex(x,y,i)].set(offset,color); }
  
  // VISUALIZATION
  void renderSamples(char *samples_file, int sample_zoom);
  void renderFilter(char *filter_file, int filter_zoom, Filter *filter);

private:

  Film() { assert(0); } // don't use this constructor
  int getIndex(int i, int j, int n) {
    assert (i >= 0 && i < width);
    assert (j >= 0 && j < height);
    assert (n >= 0 && n < num_samples);
    return i*height*num_samples + j*num_samples + n; }

  // REPRESENTATION
  int width;
  int height;
  int num_samples;
  Sample *samples;
};

// ==================================================================


class Filter {

public:
  Filter() {}
  ~Filter() {}
  Vec3f getColor(int i, int j, Film *film) {
    float weight = 0.0, w;
    Vec3f color(0,0,0);
    Vec2f offset;
    int n = film->getNumSamples();
    int r = getSupportRadius();
    int width  = film->getWidth();
    int height = film->getHeight();
    for (int x = i - r; x <= i + r; ++x) if (x >= 0 && x < width)
      for (int y = j - r; y <= j + r; ++y) if (y >= 0 && y < height)
        for (int k = 0; k < n; ++k) {
          Sample sample = film->getSample(x, y, k);
          offset = Vec2f(x-i-0.5, y-j-0.5);
          offset += sample.getPosition();
          w = getWeight(offset.x(), offset.y());
          color += sample.getColor() * w;
          weight += w;
        }
    return weight == 0.0 ? color : color * (1.0 / weight);
  }
  virtual float getWeight(float x, float y) = 0;
  virtual int getSupportRadius() = 0;
};

class BoxFilter: public Filter {
public:
  BoxFilter(float r): radius(r) {}
  ~BoxFilter() {}
  virtual float getWeight(float x, float y) {
    return fabs(x) <= radius && fabs(y) <= radius;
  }
  virtual int getSupportRadius() {return ceil(radius-0.5);}
protected:
  float radius;
};

class TentFilter: public Filter {
public:
  TentFilter(float r): radius(r) {}
  ~TentFilter() {}
  virtual float getWeight(float x, float y) {
    float p = 1.0 - sqrt(x * x + y * y) / radius;
    return p < 0.0 ? 0.0 : p;
  }
  virtual int getSupportRadius() {return ceil(radius-0.5);}
protected:
  float radius;
};

class GaussianFilter: public Filter {
public:
  GaussianFilter(float s): sigma(s) {}
  ~GaussianFilter() {}
  virtual float getWeight(float x, float y) {
    return /* sqrt(x*x + y*y) > sigma*2 ? 0.0 : */exp(-(x*x + y*y)/(2*sigma*sigma));
  }
  virtual int getSupportRadius() {return ceil(sigma*2-0.5);}
protected:
  float sigma;
};


// ==================================================================

void Film::renderSamples(char *samples_file, int sample_zoom) {

  // create an image
  Image image = Image(width*sample_zoom,height*sample_zoom);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {     
      // make a dark grey checkerboard pattern
      for (int zi = i*sample_zoom; zi < (i+1)*sample_zoom; zi++) {
        for (int zj = j*sample_zoom; zj < (j+1)*sample_zoom; zj++) {
          if (((i+j)%2)==0)
            image.SetPixel(zi,zj,Vec3f(0.2,0.2,0.2));
          else 
            image.SetPixel(zi,zj,Vec3f(0,0,0));
        }
      } 
      // draw the sample points
      for (int n = 0; n < num_samples; n++) {
        Sample s = getSample(i,j,n);
        Vec2f p = s.getPosition();
        assert (p.x() >= 0 && p.x() <= 1);
        assert (p.y() >= 0 && p.y() <= 1);
        int x = (int)floor((i+p.x())*sample_zoom);
        int y = (int)floor((j+p.y())*sample_zoom);
        assert (x >= 0);
        if (x >= width*sample_zoom) x = width*sample_zoom - 1;
        assert (y >= 0);
        if (y >= height*sample_zoom) y = height*sample_zoom - 1;
        image.SetPixel(x,y,s.getColor());
      }
    }
  }
  // save the image
  image.SaveTGA(samples_file);
}

void Film::renderFilter(char *filter_file, int filter_zoom, Filter *filter) {

  // create an image and initialize it to medium blue
  Image image = Image(width*filter_zoom+1,height*filter_zoom+1);
  image.SetAllPixels(Vec3f(0,0,0.5));

  int support_radius = filter->getSupportRadius();

  // render the filter for the center pixel
  int center = width/2;
  for (int i = center-support_radius; i <= center+support_radius; i++) {
    for (int j = center-support_radius; j <= center+support_radius; j++) {
      if (i < 0 || i >= width || j < 0 || j >= height) continue;

      // draw the sample points
      for (int i2 = 0; i2 < filter_zoom; i2++) {
        for (int j2 = 0; j2 < filter_zoom; j2++) {
          float x = i + i2/float(filter_zoom) - (center+0.5); 
          float y = j + j2/float(filter_zoom) - (center+0.5); 
          float weight = filter->getWeight(x,y);
          image.SetPixel(i*filter_zoom+i2,j*filter_zoom+j2,
            Vec3f(weight,weight,weight));
        }
      }
    }
  }
  // draw a blue grid
  for (int x = 0; x < width*filter_zoom+1; x++) {
    for (int y = 0; y < height*filter_zoom+1; y++) {
      if (x%filter_zoom == 0 ||
	  y%filter_zoom == 0)
	image.SetPixel(x,y,Vec3f(0,0,1));
    }
  }
  // save the image
  image.SaveTGA(filter_file);
}

#endif
