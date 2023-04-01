#pragma once

#include "device.hpp"
#include "window.hpp"
#include "frame_info.hpp"

// libs
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// std
#include <stdexcept>

namespace sve {

static void check_vk_result(VkResult err) {
  if (err == 0) return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
  if (err < 0) abort();
}

class SveImgui {
 public:
  SveImgui(Window &window, Device &device, VkRenderPass renderPass, uint32_t imageCount);
  ~SveImgui();

  void newFrame();

  void render(VkCommandBuffer commandBuffer);

  // Example state
  bool show_demo_window = false;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  void runExample(sve::FrameInfo info);

 private:
  Device &device;
  float f{0.f};

  // We haven't yet covered descriptor pools in the tutorial series
  // so I'm just going to create one for just imgui and store it here for now.
  // maybe its preferred to have a separate descriptor pool for imgui anyway,
  // I haven't looked into imgui best practices at all.
  VkDescriptorPool descriptorPool;
  const int hardcoded_node_id = 50;
};
}  // namespace sve
