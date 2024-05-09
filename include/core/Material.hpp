#pragma once
#include <glm/glm.hpp>

struct Material
{
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 emissionColor;
    float emissionStrength;
};