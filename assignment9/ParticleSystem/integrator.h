#ifndef _INTEGRATOR_H
#define _INTEGRATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "particle.h"
#include "forcefield.h"

class Integrator {
public:
  virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) = 0;
};

class EulerIntegrator: public Integrator {
public:
  virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) {
    glm::vec3 p(particle->getPosition());
    glm::vec3 v(particle->getVelocity());
    glm::vec3 a(forcefield->getAcceleration(p, particle->getMass(), t));
    particle->setPosition(p + v * dt);
    particle->setVelocity(v + a * dt);
    particle->increaseAge(dt);
  }
};

class MidpointIntegrator : public Integrator {
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) {
	}
};

class RungeKuttaIntegrator : public Integrator {
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) {
	}
};

#endif