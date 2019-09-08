#ifndef _FORCEFIELD_H
#define _FORCEFIELD_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ForceField {
public:
  virtual glm::vec3 getAcceleration(
    const glm::vec3 &position, float mass, float t) const = 0;
};

class GravityForceField: public ForceField {

public:
  GravityForceField(glm::vec3 g): gravity(g) {}

  virtual glm::vec3 getAcceleration(const glm::vec3 &position, float mass, float t) const {
    return gravity;
  }

private:
  glm::vec3 gravity;
};

class ConstantForceField: public ForceField {

public:
  ConstantForceField(glm::vec3 f): force(f) {}
  
  virtual glm::vec3 getAcceleration(const glm::vec3 &position, float mass, float t) const {
    return force / mass;
  }

private:
  glm::vec3 force;
};

class RadialForceField: public ForceField {
  
public:
  RadialForceField(float mag): magnitude(mag) {}
  
  virtual glm::vec3 getAcceleration(const glm::vec3 &position, float mass, float t) const {
		return -magnitude * position / mass;
  }

private:
  float magnitude;
};

class VerticalForceField: public ForceField {

public:
  VerticalForceField(float mag): magnitude(mag) {}
  
  virtual glm::vec3 getAcceleration(const glm::vec3 &position, float mass, float t) const {
    return -magnitude * glm::vec3(0.0f, position.y, 0.0f) / mass;
  }

private:
  float magnitude;
};

class WindForceField : public ForceField {
public:
	WindForceField(float mag) : magnitude(mag) {}

	virtual glm::vec3 getAcceleration(const glm::vec3 &position, float mass, float t) const {
		return glm::vec3(0.0f);
	}

private:
	float magnitude;
};

#endif