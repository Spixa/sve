#include "point_light_system.hpp"

#include <iostream>

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>
#include <array>

namespace sve {

    PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);

    }

    PointLightSystem::~PointLightSystem()
    {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

   

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        // VkPushConstantRange pushConstantRange;
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0; // todo
        // pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        // pipelineLayoutInfo.flags = 0; // exp
        // pipelineLayoutInfo.pNext = nullptr; // exp
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("[sve] failed to create pipeline layout");
        } 
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout is created");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
    
        pipelineConfig.renderPass = renderPass;  
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<sve::Pipeline>(
            device, 
            "shaders/point_light.vert.spv", 
            "shaders/point_light.frag.spv", 
            pipelineConfig
        );
    }


    void PointLightSystem::render(FrameInfo& frameInfo) {
        pipeline->bind(frameInfo.commandBuffer);

        // auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }

    
}