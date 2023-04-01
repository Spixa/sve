#ifndef SVE_SIMPLE_RENDER_SYSTEM_INCLUDED
#define SVE_SIMPLE_RENDER_SYSTEM_INCLUDED

#include "pipeline.hpp"
#include "model.hpp"
#include "gameobject.hpp"
#include "camera.hpp"
#include "frame_info.hpp"

// Additional arguments for GLM
#define GLM_FORCE_RADIANS   
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// include glm with the submitted arguments
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

// std
#include <memory>
#include <vector>


namespace sve {

    class SimpleRenderSystem {
    public:
    
    SimpleRenderSystem(sve::Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    void renderGameObjects(FrameInfo& frameInfo);

    private:

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);


    Device &device;

    std::unique_ptr<sve::Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    };
}

#endif