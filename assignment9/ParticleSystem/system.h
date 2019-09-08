#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_t.h>

#include "generator.h"
#include "integrator.h"
#include "forcefield.h"
#include "particleSet.h"
#include "particle.h"

// ====================================================================

class Particle;
class ParticleSet;
class Generator;
class Integrator;
class ForceField;

// ====================================================================
// ====================================================================
// The class for managing a particle system.

// This class keeps track of the list of particles, the 
// generator, iterator & force field of the system and
// has the main loop for updating particles.

class System {

public:

  // CONSTRUCTOR & DESTRUCTOR
  System(Generator *g, Integrator *i, ForceField *f);
  ~System();

  // FUNCTIONALITY
  void Restart();
  void Update(float dt);

	void BindShader(Shader *shader) { particles->BindShader(shader); }

  // RENDERING
  void Paint(float dt, int integrator_color, int draw_vectors, 
	     float acceleration_scale, int motion_blur) const;
  void PaintGeometry() const;


private:
  // don't use this constructor
  System() { assert(0); } 

  // REPRESENTATION
  ParticleSet *particles;
  Generator *generator;
  Integrator *integrator;
  ForceField *forcefield;
  float current_time;
};

// ====================================================================
// ====================================================================

System::System(Generator *g, Integrator *i, ForceField *f) {
  assert (g != NULL);
  assert (i != NULL);
  assert (f != NULL);
  generator = g;
  integrator = i;
  forcefield = f;
  particles = NULL;
  Restart();
}

System::~System() {
  delete generator;
  delete integrator;
  delete forcefield;
  delete particles;
}

// ====================================================================

void System::Restart() {
  // delete the current particles
  delete particles;
  particles = new ParticleSet(100);

  // restart the generator (it might have state)
  generator->Restart();

  // reset the main clock
  current_time = 0;
}

void System::Update(float dt) {
  // update each particle
  int num_particles = particles->getNumParticles();
  for (int i = 0; i < num_particles; i++) {
    integrator->Update(particles->Get(i),forcefield,current_time,dt);
  }
  // generate new particles
  int num_new = generator->numNewParticles(current_time,dt);
  for (int i = 0; i < num_new; i++) {
    Particle *p = generator->Generate(current_time,i);
    assert (p != NULL);
    particles->Add(p);
  }
  // cleanup anything that might have died!
  particles->RemoveDead();

  // increment the current time
  current_time += dt;
}

void System::PaintGeometry() const {
  // some generators have polygons
  generator->Paint();
}


void System::Paint(float dt, int integrator_color, int draw_vectors, 
		   float acceleration_scale, int motion_blur) const {

  // setup the integrator color (if appropriate)
  if (integrator_color) {

  }

  // draw the particles
  // (and the velocity vectors -- if requested)
  particles->Paint(dt, integrator_color, draw_vectors, motion_blur);

}

#endif
