#pragma once

#include "camera.hpp"
#include "gameobject.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace sve
{

  #define MAX_LIGHTS 10

  struct PointLight {
    glm::vec4 position{}; // ignore w
    glm::vec4 color{}; // w is intensity
  };

  struct GlobalUbo {
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
    glm::vec4 ambientColor{1.f, 1.f, 1.f, .04f};
    PointLight pointLights[MAX_LIGHTS];
    int numLights;
  };


  struct FrameInfo
  {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet globalDescriptorSet;
    GameObject::Map& gameObjects;
  };
};