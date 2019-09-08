#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "particleSet.h"

class Generator {

public:
	// initialization
	void SetColors(glm::vec3 c, glm::vec3 dc, float crand) {
		color = c; dead_color = dc; color_randomness = crand;
	};
	void SetLifespan(float t, float lrand, int dnp) {
		lifespan = t; lifespan_randomness = lrand; desired_num_particles = dnp;
	}
	void SetMass(float m, float mrand) {
		mass = m; mass_randomness = mrand;
	}

	// on each timestep, create some particles
	virtual int numNewParticles(float current_time, float dt) const = 0;
	virtual Particle* Generate(float current_time, int i) = 0;

	// for the gui
	void Paint() const {}
	void Restart() {}

protected:
	glm::vec3 position;
	glm::vec3 velocity;
	float position_randomness;
	float velocity_randomness;

	glm::vec3 color;
	glm::vec3 dead_color;
	float color_randomness;
	float lifespan;
	float lifespan_randomness;
	float desired_num_particles;

	float mass;
	float mass_randomness;
	Random rand;
};

class HoseGenerator: public Generator {

public:

	HoseGenerator(glm::vec3 p, float prand, glm::vec3 v, float vrand) {
		position = p; position_randomness = prand; velocity = v; velocity_randomness = vrand;
	}

	virtual int numNewParticles(float current_time, float dt) const {
		static float generated_num = 0.0f;
		generated_num += dt * desired_num_particles / lifespan;
		int new_num = (int)generated_num;
		generated_num -= new_num;
		return new_num;
	}

	virtual Particle* Generate(float current_time, int i) {
		glm::vec3 vrand = rand.randomVector();
		
		return new Particle(position, velocity + vrand, color, dead_color, mass, lifespan);
	}
};

class RingGenerator : public Generator {

public:

	RingGenerator(float prand, glm::vec3 v, float vrand) {
		position = glm::vec3(0.0f); position_randomness = prand; velocity = v; velocity_randomness = vrand;
	}

	virtual int numNewParticles(float current_time, float dt) const {
		static float generated_num = 0.0f;
		generated_num += dt * desired_num_particles / lifespan; // to edit
		int new_num = (int)generated_num;
		generated_num -= new_num;
		return new_num;
	}

	virtual Particle* Generate(float current_time, int i) {
		glm::vec3 vrand(0.0f);
		return new Particle(position, velocity + vrand, color, dead_color, mass, lifespan);
		// to edit
	}
};

#endif