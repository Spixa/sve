#include "camera.hpp"

#include <limits>
#include <GLFW/glfw3.h>
#include <cassert>

#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace sve {
  void Camera::setOrthographicProjection(
    float left, float right, float top, float bottom, float near, float far) {
    projectionMatrix = glm::mat4{1.0f};
    projectionMatrix[0][0] = 2.f / (right - left);
    projectionMatrix[1][1] = 2.f / (bottom - top);
    projectionMatrix[2][2] = 1.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix[3][2] = -near / (far - near);
  }
  
  void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovy / 2.f);
    projectionMatrix = glm::mat4{0.0f};
    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix[2][2] = far / (far - near);
    projectionMatrix[2][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);
  }

  void Camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);
  }

  void Camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    setViewDirection(position, target - position, up);
  }

  void Camera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);
  }

  void Camera::move(GLFWwindow* window, float dt) {
      
      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      {
        if (is_controlling) { is_controlling = false; firstClick = true;} 
      }

      if (is_controlling) {

        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);

        float speed_ = speed * dt * 60.f;    

        // Prevents camera from jumping on the first click
        if (firstClick)
        {
            glfwSetCursorPos(window, ((float)width / 2), ((float)height / 2));
            firstClick = false;
            rotation_ = { 0.0f, 0.0f, 0.0f };
        }

        // Handles key inputs
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            position_ += speed_  * -orientation_;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            position_ += speed_ * -glm::normalize(glm::cross(orientation_, up_));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            position_ += speed_ * orientation_;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            position_ += speed_ * glm::normalize(glm::cross(orientation_, up_));
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            position_ += speed_ * -up_;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            position_ += speed_ * up_;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            speed = 0.01f;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
        {
            speed = 0.1f;
        }

        // Stores the coordinates of the cursor
        double mouseX;
        double mouseY;
        // Fetches the coordinates of the cursor
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
        // and then "transforms" them into degrees
        float rotX = sensitivity * (float)(mouseY - ((float)height / 2)) / height;
        float rotY = sensitivity * -(float)(mouseX - ((float)width / 2)) / width;

        // Calculates upcoming vertical change in the Orientation
        glm::vec3 newOrientation = glm::rotate(orientation_, glm::radians(-rotX), glm::normalize(glm::cross(orientation_, up_)));

        // Decides whether or not the next vertical Orientation is legal or not
        if (std::abs(glm::angle(newOrientation, up_) - glm::radians(90.0f)) <= glm::radians(180.0f))
        {
            orientation_ = newOrientation;
        }

        // Rotates the Orientation left and right
        orientation_ = glm::rotate(orientation_, glm::radians(-rotY), up_);

        // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
        glfwSetCursorPos(window, ((float)width / 2), ((float)height / 2));
        viewMatrix = glm::lookAt(position_, position_ + orientation_, up_);
      } else {

      }
  }
};