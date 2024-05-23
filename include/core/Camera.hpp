
/** @file Camera.hpp
 *  @brief Sets up an OpenGL camera.
 *
 *  Sets up an OpenGL Camera. The camera is what
 *  sets up our 'view' matrix.
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#pragma once

#include "glm/glm.hpp"

class Camera {
public:
  // Constructor to create a camera
  Camera(int windowWidth, int windowHeight, float fov = 90.0f,
         float nearPlane = 0.1f, float farPlane = 1000.0f);

  // Return a 'view' matrix with our camera transformation applied.
  glm::mat4 getViewMatrix() const;
  // Return the projection matrix for our camera
  const glm::mat4 &getProjectionMatrix() const { return _projectionMatrix; }

  // Move the camera around
  void mouseLook(int mouseX, int mouseY);
  void turn(float yaw, float pitch);
  void moveForward(float speed);
  void moveBackward(float speed);
  void moveLeft(float speed);
  void moveRight(float speed);
  void moveUp(float speed);
  void moveDown(float speed);

  // Look at a target
  void lookAt(const glm::vec3 &target);

  // Setters
  template <typename... Args> void setPosition(Args &&...args) {
    _position = glm::vec3(std::forward<Args>(args)...);
  }
  void setViewDirection(const glm::vec3 &viewDirection) {
    _viewDirection = viewDirection;
  }
  void setUpVector(const glm::vec3 &upVector) { _upVector = upVector; }

  // Get the position of the camera
  glm::vec3 &getPosition() { return _position; }
  const glm::vec3 &getPosition() const { return _position; }

  // Get the direction the camera is looking
  glm::vec3 &getViewDirection() { return _viewDirection; }
  const glm::vec3 &getViewDirection() const { return _viewDirection; }

  // Get the up vector for the camera
  glm::vec3 &getUpVector() { return _upVector; }
  const glm::vec3 &getUpVector() const { return _upVector; }

private:
  // Track the old mouse position
  glm::vec2 _oldMousePosition;
  // Where is our camera positioned
  glm::vec3 _position{0.0f, 0.0f, 0.0f};
  // What direction is the camera looking
  glm::vec3 _viewDirection{0.0f, 0.0f, -1.0f};
  // Which direction is 'up' in our world
  glm::vec3 _upVector{0.0f, 1.0f, 0.0f};
  // The projection matrix for our camera
  glm::mat4 _projectionMatrix;
};
