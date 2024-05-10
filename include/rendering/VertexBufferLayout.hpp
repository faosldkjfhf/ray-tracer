#ifndef VERTEX_BUFFER_LAYOUT_HPP
#define VERTEX_BUFFER_LAYOUT_HPP
/** @file VertexBufferLayout.hpp
 *  @brief Sets up a variety of Vertex Buffer Object (VBO) layouts.
 *
 *  More...
 *
 *  @author Mike
 *  @bug No known bugs.
 */

// The glad library helps setup OpenGL extensions.
#include <glad/glad.h>

#include <vector>

struct MeshVertex;

class VertexBufferLayout {
public:
  // Vertex Array Object
  GLuint vao = 0;
  // Vertex Buffer
  GLuint vbo = 0;
  // Index Buffer Object
  GLuint ebo = 0;

  VertexBufferLayout() = default;

  // Destroys all of our buffers.
  ~VertexBufferLayout();

  // Selects the buffer to bind
  // We only need to bind to a buffer
  // again if we are updating the data.
  void bind() const;
  // Unbind our buffers
  void unbind() const;

  // A normal map layout needs the following attributes
  // positions: x,y,z
  // normals:  x,y,z
  // texcoords: s,t
  // tangent: t_x,t_y,t_z
  // bitangent b_x,b_y,b_z
  void createBufferLayout(std::vector<MeshVertex> &vertices,
                          std::vector<unsigned int> &indices);
};

#endif
