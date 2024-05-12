#pragma once

#include <glm/glm.hpp>

struct GpuObject {
  glm::vec4 data;
  alignas(16) unsigned int type;
  unsigned int materialIdx;
};