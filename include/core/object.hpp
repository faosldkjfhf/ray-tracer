#pragma once

#include "glm/vec3.hpp"

class Object {
public:
  Object();
  ~Object();

private:
  glm::vec3 _position;
  glm::vec3 _scale;
  float _t0;
};
