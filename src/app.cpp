#include "app.hpp"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <array>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <vulkan/vulkan_core.h>

#include "descriptors.hpp"
#include "gameobject.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "sve_imgui.hpp"
#include "swapchain.hpp"
#include "utils.hpp"



namespace sve {


    App::App()
    {
        init_imgui();
        globalPool = DescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        
        loadGameObjects();

    }

    App::~App()
    {   

    }

    void App::run() {
        
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<Buffer>(
                device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );

            uboBuffers[i]->map();
        } 

        auto globalSetLayout = DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }
            

        sve::SveImgui imgui{
            window,
            device,
            renderer.getSwapChainRenderPass(),
            renderer.getImageCount()    
        };

        sve::SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        sve::PointLightSystem pointLightSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

        sve::Camera camera{};
                                                      /* 0.f 0.f 2.5f is cubes position */
        camera.setViewTarget(glm::vec3(-1, -2.5, 2.f), glm::vec3(0.f, 0.f, 2.5f));


        // KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        // glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_ENABLED);

        while (!window.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, 120.f);

            // camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            camera.move(window.getGLFWwindow(), frameTime);

            float aspect = renderer.getAspectRatio();

            camera.setPerspectiveProjection(glm::radians(70.f), aspect, .1f, 100.f);

            if (auto commandBuffer = renderer.beginFrame())
            {
                int frameIndex = renderer.getFrameindex();

                FrameInfo frameInfo
                {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // update:  
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //render
                imgui.newFrame();

                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                imgui.runExample(frameInfo);
                imgui.render(commandBuffer);
                
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            } 
        }

        vkDeviceWaitIdle(device.device());
    }


    void App::loadGameObjects()
    {
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "../models/flat_vase.obj");
        auto flat_vase = GameObject::createGameObject();
        flat_vase.model = model;
        flat_vase.transform.scale = glm::vec3{3.f};
        flat_vase.transform.translation = {-.5f, .5f, 0.f};
        gameObjects.emplace(flat_vase.getId(), std::move(flat_vase));

        model = Model::createModelFromFile(device, "../models/Kord Machine Gun.obj");
        auto machinegun = GameObject::createGameObject();
        machinegun.model = model;
        machinegun.transform.scale = glm::vec3{0.01f};
        machinegun.transform.translation = {-1., 0.f, 0.f};
        gameObjects.emplace(machinegun.getId(), std::move(machinegun)); 

        model = Model::createModelFromFile(device, "../models/quad.obj");
        auto floor = GameObject::createGameObject();
        floor.model = model;
        floor.transform.scale = glm::vec3{3.0f, 1.0f, 3.0f};
        floor.transform.translation = {0., 0.5f, 0.f};
        gameObjects.emplace(floor.getId(), std::move(floor));

    }

    void App::init_imgui()
    {
                
    }
};
