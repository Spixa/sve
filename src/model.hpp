#ifndef SVE_MODEL_INCLUDED
#define SVE_MODEL_INCLUDED

#include "device.hpp"
#include "buffer.hpp"

#include <vector>

// Additional arguments for GLM
#define GLM_FORCE_RADIANS   
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// include glm with the submitted arguments
#include "glm/glm.hpp"

#include <memory>

namespace sve 
{

    class Model {
        public:
    
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};
            
            bool operator==(const Vertex& other) const
            {
                return
                    position == other.position &&
                    color == other.color &&
                    normal == other.normal &&
                    uv == other.uv;
            }

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string& filePath);
        };

        Model(Device& device, const Model::Builder& builder);
        ~Model();

        Model(Model const&) = delete;
        Model& operator=(Model const&) = delete;

        static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filePath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);
        private:
        void createVertexBuffers(const std::vector<Vertex> &verticesArgument);
        void createIndexBuffers(const std::vector<uint32_t> &indices);


        std::unique_ptr<Buffer> vertexBuffer;
        uint32_t vertexCount;

        sve::Device& device;

        bool hasIndexBuffer{false};
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t indexCount;        
    };
};

#endif