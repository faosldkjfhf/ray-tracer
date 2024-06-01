#include "core/Camera.hpp"

#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(int windowWidth, int windowHeight, float fov, float nearPlane,
               float farPlane) {
  _projectionMatrix = glm::perspective(glm::radians(fov),
                                       (float)windowWidth / (float)windowHeight,
                                       nearPlane, farPlane);
}

void Camera::mouseLook(int mouseX, int mouseY) {
  // Record our new position as a vector
  glm::vec2 newMousePosition(mouseX, mouseY);
  static bool firstLook = true;
  // Note: we need to handle the 'first' mouse event
  // Note: We should also 'center' the mouse in our
  //       program when we execute it.
  if (true == firstLook) {
    firstLook = false;
    _oldMousePosition = newMousePosition;
  }
  // Detect how much the mouse has moved since
  // the last time
  const float MOUSE_SENSITIVITY = 0.2f;
  glm::vec2 mouseDelta = _oldMousePosition - newMousePosition;
  mouseDelta.x *= MOUSE_SENSITIVITY;
  mouseDelta.y *= MOUSE_SENSITIVITY;

  // Update the view direction around the up vector
  _viewDirection =
      glm::rotate(_viewDirection, glm::radians(mouseDelta.x), _upVector);

  // Compute the rightVector
  glm::vec3 rightVector = glm::cross(_viewDirection, _upVector);
  _viewDirection =
      glm::rotate(_viewDirection, glm::radians(mouseDelta.y), rightVector);

  // Update our old position after we have made changes
  _oldMousePosition = newMousePosition;
}

void Camera::turn(float yaw, float pitch) {
  _viewDirection = glm::rotate(_viewDirection, glm::radians(yaw), _upVector);
  glm::vec3 rightVector = glm::cross(_viewDirection, _upVector);
  _viewDirection =
      glm::rotate(_viewDirection, glm::radians(pitch), rightVector);
}

void Camera::moveForward(float speed) { _position += _viewDirection * speed; }

void Camera::moveBackward(float speed) { _position -= _viewDirection * speed; }

void Camera::moveLeft(float speed) {
  // Compute the rightVector
  glm::vec3 rightVector = glm::cross(_viewDirection, _upVector);
  _position -= rightVector * speed;
}

void Camera::moveRight(float speed) {
  // Compute the rightVector
  glm::vec3 rightVector = glm::cross(_viewDirection, _upVector);
  _position += rightVector * speed;
}

void Camera::moveUp(float speed) { _position += _upVector * speed; }

void Camera::moveDown(float speed) { _position -= _upVector * speed; }

void Camera::lookAt(const glm::vec3 &target) {
  _viewDirection = glm::normalize(target - _position);
}

glm::mat4 Camera::getViewMatrix() const {
  // Think about the second argument and why that is
  // setup as it is.
  return glm::lookAt(_position, _position + _viewDirection, _upVector);
}
