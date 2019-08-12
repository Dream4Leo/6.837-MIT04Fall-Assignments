#ifndef _SAMPLER_H
#define _SAMPLER_H


#include "vectors.h"
#include <stdlib.h>


class Sampler {

public:
  Sampler() {}
  ~Sampler() {}
  virtual Vec2f getSamplerPosition(int n) = 0;

protected:
  float rand01() {return rand()%1000/1000.0;}
  int num_samples;
};

class RandomSampler: public Sampler {

public:
  RandomSampler(int n) {num_samples = n;}
  ~RandomSampler() {}
  virtual Vec2f getSamplerPosition(int n) {
    return Vec2f(rand01(), rand01());
  }

};

class UniformSampler: public Sampler {

public:
  UniformSampler(int n) {
    num_samples = n;
    c = sqrt(n);
    delta = 1.0 / c;
  }
  ~UniformSampler() {}
  virtual Vec2f getSamplerPosition(int n) {
    return Vec2f(delta * (n % c + 0.5), delta * (n / c + 0.5));
  }
private:
  int   c;
  float delta;
};

class JitteredSampler: public Sampler {

public:
  JitteredSampler(int n) {
    num_samples = n;
    c = sqrt(n);
    delta = 1.0 / c;
  }
  ~JitteredSampler() {}
  virtual Vec2f getSamplerPosition(int n) {
    return Vec2f(delta * (n % c + rand01()), delta * (n / c + rand01()));
  }
private:
  int   c;
  float delta;
};

#endif