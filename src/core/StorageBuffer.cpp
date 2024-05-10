#include "core/StorageBuffer.hpp"

StorageBuffer::~StorageBuffer() { glDeleteBuffers(1, &ssbo); }

void StorageBuffer::bind() const {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
}

void StorageBuffer::unbind() const {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
