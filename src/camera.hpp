#ifndef SVE_CAMERA_INCLUDED
#define SVE_CAMERA_INCLUDED

// Additional arguments for GLM
#define GLM_FORCE_RADIANS   
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// include glm with the submitted arguments
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>

namespace sve {
  class Camera {
    public:
    void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
    void setPerspectiveProjection(float fovy, float aspect, float near, float far);

    void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

    void move(GLFWwindow* window, float dt);

    const glm::mat4& getProjection() const { return projectionMatrix; }
    const glm::mat4& getView() const { return viewMatrix; }

    void setControl(bool tof) { is_controlling = tof; }
    void setSensitivity(float def = 120.0f)
    {
      sensitivity = def;
    }

    private:
      glm::mat4 viewMatrix{1.f};
      glm::mat4 projectionMatrix{1.f};
      
      glm::vec3 position_ = { 0.0f, -1.0f, 0.0f };
      glm::vec3 rotation_ = { 0.0f, 0.0f, 0.0f };
      glm::vec3 orientation_ = { -1.0f, -1.0f, -1.0f };
      glm::vec3 up_ = { 0.0f, 1.0f, 0.0f };

      bool firstClick = true;
      bool is_controlling = true; 
      float speed = 0.1f;
      float sensitivity = 120.0f;
  };  
};

#endif