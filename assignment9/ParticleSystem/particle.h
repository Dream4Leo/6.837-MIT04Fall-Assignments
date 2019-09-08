#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particle {
public:

  // CONSTRUCTOR & DESTRUCTOR
  Particle(glm::vec3 p, glm::vec3 v, glm::vec3 _color, glm::vec3 _dead_color, float m, float lifespan):
    position(p), last_position(p), velocity(v), color(_color), 
    dead_color(_dead_color), mass(m), timeToDeath(lifespan) {}
  ~Particle() {}

  // ACCESSORS
  glm::vec3 getPosition() const { return position; }
  glm::vec3 getVelocity() const { return velocity; }
  glm::vec3 getColor() const { return color; }
  float getMass() const { return mass; }  
  int isDead() const { if (timeToDeath <= 0) return 1; else return 0; }

  // MODIFIER
  void setPosition(glm::vec3 p) { last_position = position; position = p; }
  void setVelocity(glm::vec3 v) { velocity = v; }
  void setColor(glm::vec3 c) { color = c; }
  void setMass(float m) { mass = m; }
  void increaseAge(float a);

protected:

  Particle() { assert(0); } // don't use this constructor

  // REPRESENTATION
  glm::vec3 position;
  glm::vec3 last_position; 
  glm::vec3 velocity;
  glm::vec3 color;
  glm::vec3 dead_color; // color interpolates from color to dead_color as clock ticks down
  float mass;
  float timeToDeath;

};

void Particle::increaseAge(float a) { 
  // interpolate towards dead_color as the clock ticks down  
  float t = 1.0f;
  if (timeToDeath > 0.0f) t = a / timeToDeath;
  if (t > 1) t = 1.0f;
  color += t*(dead_color-color); 
  // adjust the clock
  timeToDeath -= a; 
}

#endif
