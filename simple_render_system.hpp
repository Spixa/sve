#ifndef SVE_SIMPLE_RENDER_SYSTEM_INCLUDED
#define SVE_SIMPLE_RENDER_SYSTEM_INCLUDED

#include "pipeline.hpp"
#include "model.hpp"
#include "gameobject.hpp"
#include "camera.hpp"

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
    struct SimplePushConstantData {
        glm::mat4 transform{1.f};
        alignas(16) glm::vec3 color;
    };

    class SimpleRenderSystem {
    public:
    
    SimpleRenderSystem(sve::Device& device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<sve::GameObject> &gameObjects, const sve::Camera& camera);

    private:

    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);


    Device &device;

    std::unique_ptr<sve::Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    };
}

#endif