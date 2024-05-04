#pragma once

#include "core/Object.hpp"
#include <vector>

class Scene {
public:
  Scene() = default;

private:
  std::vector<Object> _objects;
};
