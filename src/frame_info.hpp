#pragma once

#include "camera.hpp"
#include "gameobject.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace sve
{
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