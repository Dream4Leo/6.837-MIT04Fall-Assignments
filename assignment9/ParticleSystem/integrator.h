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
		float m = particle->getMass();
		glm::vec3 p(particle->getPosition());
		glm::vec3 v(particle->getVelocity());
		glm::vec3	pm = p + v * dt / 2.0f;
		glm::vec3	vm = v + forcefield->getAcceleration(p, m, t) * dt / 2.0f;
		glm::vec3 am = forcefield->getAcceleration(pm, m, t + dt / 2.0f);
		particle->setPosition(p + vm * dt);
		particle->setVelocity(v + am * dt);
	}
};

class RungeKuttaIntegrator : public Integrator {
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) {
		float m = particle->getMass();
		glm::vec3 p(particle->getPosition());
		glm::vec3 v(particle->getVelocity());
		glm::vec3 a(forcefield->getAcceleration(p, m, t));

		glm::vec3 p1 = p + v * dt;
		glm::vec3 v1 = v + a * dt;
		glm::vec3 a1(forcefield->getAcceleration(p1, m, t + dt));

		glm::vec3 p2 = p1 + v1 * dt;
		glm::vec3 v2 = v1 + a1 * dt;

		particle->setPosition((p1 + p2) / 2.0f);
		particle->setVelocity((v1 + v2) / 2.0f);
	}
};

#endif