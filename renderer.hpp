#ifndef SVE_RENDERER_INCLUDED
#define SVE_RENDERER_INCLUDED

#include "window.hpp"
#include "swapchain.hpp"
#include "model.hpp"

// Additional arguments for GLM
#define GLM_FORCE_RADIANS   
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// include glm with the submitted arguments
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>


namespace sve {



    class Renderer {
    public:
    

    Renderer(Window& window, Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    VkCommandBuffer beginFrame();
    void endFrame();

    VkRenderPass getSwapChainRenderPass() const  { return swapchain->getRenderPass(); }
    float getAspectRatio() const { return swapchain->extentAspectRatio(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const { 
      assert(isFrameStarted && "Cannot get command buffer when the frame is not started.");
      return commandBuffers[currentFrameIndex];
    }

    int getFrameindex() const { 
      assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
      return currentImageIndex; 
    }

    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
    void createCommandBuffers();
    void freeCommandBuffers();

    void recreateSwapChain();

    sve::Window& window;
    Device& device;

    std::unique_ptr<sve::SwapChain> swapchain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};

    };
}


#endif ///