#pragma once
#include "core/object.hpp"
#include <vector>

class Scene {
public:
  Scene();
  ~Scene();

private:
  std::vector<Object> _objects;
};
