#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "vectors.h"


// Included files for OpenGL Rendering
#include <GL/gl.h>
#include <GL/glut.h>

// ================================================================
//
// stores the information for a particle

class Particle {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Particle(Vec3f p, Vec3f v, Vec3f _color, Vec3f _dead_color, float m, float lifespan) {
    position = p;
    last_position = p;
    velocity = v;
    color = _color;
    dead_color = _dead_color;
    mass = m;
    timeToDeath = lifespan; }
  ~Particle() {}

  // ACCESSORS
  Vec3f getPosition() const { return position; }
  Vec3f getVelocity() const { return velocity; }
  Vec3f getColor() const { return color; }
  float getMass() const { return mass; }  
  int isDead() const { if (timeToDeath <= 0) return 1; else return 0; }

  // MODIFIER
  void setPosition(Vec3f p) { last_position = position; position = p; }
  void setVelocity(Vec3f v) { velocity = v; }
  void setColor(Vec3f c) { color = c; }
  void setMass(float m) { mass = m; }
  void increaseAge(float a);

  // Paint
  void Paint(float dt, int integrator_color, int draw_vectors, int motion_blur) const;
  
private:

  Particle() { assert(0); } // don't use this constructor

  // REPRESENTATION
  Vec3f position;
  Vec3f last_position; 
  Vec3f velocity;
  Vec3f color;
  Vec3f dead_color; // color interpolates from color to dead_color as clock ticks down
  float mass;
  float timeToDeath;

};

// ================================================================


void Particle::increaseAge(float a) { 
  // interpolate towards dead_color as the clock ticks down  
  float t = 1;
  if (timeToDeath > 0) 
    t = a / timeToDeath;
  if (t > 1) t = 1;
  color += t*(dead_color-color); 

  // adjust the clock
  timeToDeath -= a; 
}


void Particle::Paint(float dt, int integrator_color, int draw_vectors, int motion_blur) const {

  // grab the particle color
  if (integrator_color == 0) {
    glColor3f(color.x(),color.y(),color.z());
  }

  // use slightly larger points if it's not motion blurred
  if (motion_blur == 0) {
    glPointSize(3);
    glBegin(GL_POINTS);
    glVertex3f(position.x(),position.y(),position.z());
    glEnd();
  }

  // draw the velocity vectors
  if (draw_vectors == 1) {
    glLineWidth(1);
    glBegin(GL_LINES);
    Vec3f a = position;
    Vec3f b = position + dt*velocity;
    glVertex3f(a.x(),a.y(),a.z());
    glVertex3f(b.x(),b.y(),b.z());
    glEnd();
  }

  // draw motion blur lines
  if (motion_blur == 1) {
    glLineWidth(1);
    glBegin(GL_LINES);
    Vec3f a = position;
    Vec3f b = last_position;
    glVertex3f(a.x(),a.y(),a.z());
    glVertex3f(b.x(),b.y(),b.z());
    glEnd();
  }
}



#endif
