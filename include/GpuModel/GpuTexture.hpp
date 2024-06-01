#pragma once

struct GpuTexture {
  alignas(16) int diffuseTextureIdx = -1; // -1 if no texture
  int normalTextureIdx = -1;              // -1 if no texture
};
