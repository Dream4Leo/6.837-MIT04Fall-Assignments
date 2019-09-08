#ifndef _PARSER_H_
#define _PARSER_H_

#include <assert.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "system.h"
#include "generator.h"
#include "integrator.h"
#include "forcefield.h"

// ====================================================================
// ====================================================================

#define MAX_PARSER_TOKEN_LENGTH 100

class System;
class Generator;
class Integrator;
class ForceField;

// ====================================================================
// ====================================================================

// parse the particle system file

class Parser {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Parser(const char *file);
  ~Parser();

  // ACCESSORS
  int getNumSystems() { return num_systems; }
  System* getSystem(int i) {
    assert (i >= 0 && i < num_systems);
    return systems[i]; }

private:
  // don't use this constructor
  Parser() { assert(0); } 

  // HELPER FUNCTIONS
  System* ParseSystem();
  Generator* ParseGenerator();
  Integrator* ParseIntegrator();
  ForceField* ParseForceField();

  // UTILITY FUNCTIONS
  int getToken(char token[MAX_PARSER_TOKEN_LENGTH]);
  glm::vec3 readVec3f();
  glm::vec2 readVec2f();
  float readFloat();
  int readInt();

  // REPRESENTATION
  int num_systems;
  System **systems;
  FILE *file;
};

// ====================================================================
// ====================================================================

Parser::Parser(const char *filename) {
  // open the file for reading
  assert (filename != NULL);
  file = fopen(filename,"r");
  assert (file != NULL);
  char token[MAX_PARSER_TOKEN_LENGTH];

  // read in the number of particles in this file
  getToken(token);
  assert (!strcmp(token,"num_systems"));
  num_systems = readInt();
  systems = new System*[num_systems];

  // read the systems
  for (int i = 0; i < num_systems; i++) {
    System *s = ParseSystem();
    assert (s != NULL);
    systems[i] = s;
  }
  fclose(file);
}


Parser::~Parser() {
  // cleanup
  for (int i = 0; i < num_systems; i++) {
    delete systems[i]; }
  delete [] systems;
}

// ====================================================================

System* Parser::ParseSystem() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert (!strcmp(token,"system"));
  Generator *generator = ParseGenerator();
  Integrator *integrator = ParseIntegrator(); 
  ForceField *forcefield = ParseForceField(); 
  return new System(generator, integrator, forcefield);
}

// ====================================================================
// ====================================================================
// ====================================================================

Generator* Parser::ParseGenerator() {
  // read the generator type
  char type[MAX_PARSER_TOKEN_LENGTH];
  getToken(type);

  // generator variable defaults
  glm::vec3 position(0.0f);
  float position_randomness = 0.0f;
  glm::vec3 velocity(0.0f);
  float velocity_randomness = 0.0f;
	glm::vec3 color(1.0f);
	glm::vec3 dead_color(1.0f);
  float color_randomness = 0.0f;
  float mass = 1.0f;
  float mass_randomness = 0.0f;
  float lifespan = 10.0f;
  float lifespan_randomness = 0.0f;
  int desired_num_particles = 1000;
  int grid = 10;
  float ring_velocity = 1.0f;

  // read the provided values
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert (!strcmp(token,"{"));
  while (1) {
    getToken(token);
    if (!strcmp(token,"position")) {
      position = readVec3f();
    } else if (!strcmp(token,"position_randomness")) {
      position_randomness = readFloat();
    } else if (!strcmp(token,"velocity")) {
      velocity = readVec3f();
    } else if (!strcmp(token,"velocity_randomness")) {
      velocity_randomness = readFloat();
    } else if (!strcmp(token,"color")) {
      color = readVec3f();
      dead_color = color;
    } else if (!strcmp(token,"dead_color")) {
      dead_color = readVec3f();
    } else if (!strcmp(token,"color_randomness")) {
      color_randomness = readFloat();
    } else if (!strcmp(token,"mass")) {
      mass = readFloat();
    } else if (!strcmp(token,"mass_randomness")) {
      mass_randomness = readFloat();
    } else if (!strcmp(token,"lifespan")) {
      lifespan = readFloat();
    } else if (!strcmp(token,"lifespan_randomness")) {
      lifespan_randomness = readFloat();
    } else if (!strcmp(token,"desired_num_particles")) {
      desired_num_particles = readInt();
    } else if (strcmp(token,"}")) {
      printf ("ERROR unknown generator token %s\n", token);
      assert(0);
    } else {
      break;
    }
  }

  // create the appropriate generator
  Generator *answer = NULL;
  if (!strcmp(type,"hose_generator")) {
    answer = new HoseGenerator(position,position_randomness,
			       velocity,velocity_randomness);
  } else if (!strcmp(type,"ring_generator")) {
    answer = new RingGenerator(position_randomness,velocity,velocity_randomness);
  } else {
    printf ("WARNING:  unknown generator type '%s'\n", type);
  }

  // set the common generator parameters
  assert (answer != NULL);
  answer->SetColors(color,dead_color,color_randomness);
  answer->SetMass(mass,mass_randomness);
  answer->SetLifespan(lifespan,lifespan_randomness,desired_num_particles);

  return answer;
}

// ====================================================================
// ====================================================================
// ====================================================================

Integrator* Parser::ParseIntegrator() {
  // read the integrator type
  char type[MAX_PARSER_TOKEN_LENGTH];
  getToken(type);

  // there are no variables
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token,"{"));
  getToken(token);
  assert(!strcmp(token,"}"));

  // create the appropriate integrator
  Integrator *answer = NULL;
  if (!strcmp(type,"euler_integrator")) {
    answer = new EulerIntegrator();
  } else if (!strcmp(type,"midpoint_integrator")) {
    answer = new MidpointIntegrator();
  } else if (!strcmp(type,"rungekutta_integrator")) {
    answer = new RungeKuttaIntegrator();
  } else {
    printf ("WARNING:  unknown integrator type '%s'\n", type);
  }
  assert (answer != NULL);
  return answer;
}

// ====================================================================
// ====================================================================
// ====================================================================

ForceField* Parser::ParseForceField() {
  // read the forcefield type
  char type[MAX_PARSER_TOKEN_LENGTH];
  getToken(type);

  // forcefield variable defaults
  glm::vec3 gravity(0.0f);
	glm::vec3 force(0.0f);
  float magnitude = 1.0f;

  // read the provided values
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert (!strcmp(token,"{"));
  while (1) {
    getToken(token);
    if (!strcmp(token,"gravity")) {
      gravity = readVec3f();
    } else if (!strcmp(token,"force")) {
      force = readVec3f();
    } else if (!strcmp(token,"magnitude")) {
      magnitude = readFloat();
    } else if (strcmp(token,"}")) {
      printf ("ERROR unknown gravity token %s\n", token);
      assert(0);
    } else {
      break;
    }
  }

  // create the appropriate force field
  ForceField *answer = NULL;
  if (!strcmp(type,"gravity_forcefield")) {
    answer = new GravityForceField(gravity);
  } else if (!strcmp(type,"constant_forcefield")) {
    answer = new ConstantForceField(force);
  } else if (!strcmp(type,"radial_forcefield")) {
    answer = new RadialForceField(magnitude);
  } else if (!strcmp(type,"vertical_forcefield")) {
    answer = new VerticalForceField(magnitude);
  } else if (!strcmp(type,"wind_forcefield")) {
    answer = new WindForceField(magnitude);
  } else {
    printf ("WARNING:  unknown forcefield type '%s'\n", type);
  }
  assert (answer != NULL);
  return answer;
}

// ====================================================================
// ====================================================================
// HELPER FUNCTIONS

int Parser::getToken(char token[MAX_PARSER_TOKEN_LENGTH]) {
  // for simplicity, tokens must be separated by whitespace
  assert (file != NULL);
  int success = fscanf(file,"%s ",token);
  if (success == EOF) {
    token[0] = '\0';
    return 0;
  }
  return 1;
}


glm::vec3 Parser::readVec3f() {
  float x,y,z;
  int count = fscanf(file,"%f %f %f",&x,&y,&z);
  if (count != 3) {
    printf ("Error trying to read 3 floats to make a Vec3f\n");
    assert (0);
  }
  return glm::vec3(x,y,z);
}


glm::vec2 Parser::readVec2f() {
  float u,v;
  int count = fscanf(file,"%f %f",&u,&v);
  if (count != 2) {
    printf ("Error trying to read 2 floats to make a Vec2f\n");
    assert (0);
  }
  return glm::vec2(u,v);
}


float Parser::readFloat() {
  float answer;
  int count = fscanf(file,"%f",&answer);
  if (count != 1) {
    printf ("Error trying to read 1 float\n");
    assert (0);
  }
  return answer;
}


int Parser::readInt() {
  int answer;
  int count = fscanf(file,"%d",&answer);
  if (count != 1) {
    printf ("Error trying to read 1 int\n");
    assert (0);
  }
  return answer;
}

// ====================================================================


#endif
