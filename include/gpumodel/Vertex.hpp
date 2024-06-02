#pragma once

#include <glm/glm.hpp>

struct Vertex {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec2 uv;
};
