#ifndef SVE_MODEL_INCLUDED
#define SVE_MODEL_INCLUDED

#include "device.hpp"
#include <vector>

// Additional arguments for GLM
#define GLM_FORCE_RADIANS   
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// include glm with the submitted arguments
#include "glm/glm.hpp"


namespace sve 
{

    class Model {
        public:
    
        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;
            
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        Model(Device& device, const std::vector<Vertex> &verticesArgument );
        ~Model();

        Model(Model const&) = delete;
        Model& operator=(Model const&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);
        private:
        void createVertexBuffers(const std::vector<Vertex> &verticesArgument);


        VkBuffer vertexBuffer;
        sve::Device& device;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    };
};

#endif