#ifndef _PARTICLE_SET_
#define _PARTICLE_SET_

#include <assert.h>
#include <shader_t.h>

#include "particle.h"

class ParticleSet {

public:
  // CONSTRUCTOR & DESTRUCTOR
ParticleSet(int n) {
  size = n;
  num_particles = 0;
	particles = new Particle*[size];
	vertices = new float[size*6];
  for (int i = 0; i < size; i++) {
    particles[i] = NULL; 
	}
	glGenBuffers(1, &particleVBO);
	glGenVertexArrays(1, &particleVAO);
}

~ParticleSet() {
  for (int i = 0; i < num_particles; i++) {
    delete particles[i]; 
	}
  delete [] particles;
	delete [] vertices;
}

// ACCESSORS
int getNumParticles() const { return num_particles; }
Particle* Get(int i) const {
  assert (i >= 0 && i < num_particles);
  assert (particles[i] != NULL);
  return particles[i]; 
}

// MODIFIERS
void Add(Particle *p) {
  assert (p != NULL);
  if (num_particles == size) {
    // double the space & copy 
    Particle **new_particles = new Particle*[size*2];
		float *new_vertices = new float[size*2*6];
		for (int i = 0; i < size; ++i) {
			new_particles[i] = particles[i];
			new_particles[i + size] = NULL;
		}
			
    delete [] particles;
		delete [] vertices;
    particles = new_particles;
		vertices = new_vertices;
    size <<= 1;
  }
  assert (num_particles < size);
	particles[num_particles] = p;
	num_particles++;

}

void RemoveDead() {
  int i = 0;
  while (1) {
    if (i == num_particles) break;
    assert (particles[i] != NULL); // [TO FIX] ! ! !  ABORTED AFTER /FIRST/ CALL TO THIS FUNCTION
    if (particles[i]->isDead()) {
			// delete this particle 
			delete particles[i];
			num_particles--;
			// & copy the last particle to this spot
			particles[i] = particles[num_particles];
			particles[num_particles] = NULL;
    } else {
			i++;
    }
  }
  // verify cleanup
  for (i = 0; i < size; i++) {
    if (i < num_particles) {
			assert (particles[i] != NULL);
			assert (!particles[i]->isDead());
    } else {
			assert (particles[i] == NULL);
    }
  }
}

void Paint(float dt, int integrator_color, int draw_vectors, int motion_blur) {
	
	for (int i = 0; i < num_particles; ++i) {
		glm::vec3 pos = particles[i]->getPosition();
		glm::vec3 col = particles[i]->getColor();
		vertices[i*6+0] = pos.x; vertices[i*6+1] = pos.y; vertices[i*6+2] = pos.z;
		vertices[i*6+3] = col.x; vertices[i*6+4] = col.y; vertices[i*6+5] = col.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
	glBufferData(GL_ARRAY_BUFFER, num_particles * 6 * sizeof(float), vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(particleVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	particleShader->use();
	glDrawArrays(GL_POINTS, 0, num_particles);
}

void BindShader(Shader *shader) {
	particleShader = shader;
}

private:
  // don't use this constructor
  ParticleSet() { assert(0); }
  // REPRESENTATION
  int num_particles;
  int size;
	float *vertices;
	Particle **particles;
	Shader *particleShader;
	unsigned int particleVAO;
	unsigned int particleVBO;
};

#endif
