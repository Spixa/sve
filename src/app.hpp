#ifndef SVE_APP_INCLUDED
#define SVE_APP_INCLUDED
#include "descriptors.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "model.hpp"
#include "gameobject.hpp"
#include "camera.hpp"
#include "buffer.hpp"
#include <glm/geometric.hpp>


// Additional arguments for GLM
#define GLM_FORCE_RADIANS   
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// include glm with the submitted arguments
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include "imgui.h"

// std
#include <memory>
#include <vector>



namespace sve {
    class App {
    public:
    
    static constexpr int WIDTH = 1280, HEIGHT = 720;

    App();
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;


    void run();
    private:
    void loadGameObjects();
    void init_imgui();

    sve::Window window{WIDTH, HEIGHT, "SVE"};

    Device device{window};

    std::unique_ptr<DescriptorPool> globalPool{};
    sve::Renderer renderer{window, device};
    GameObject::Map gameObjects;

};

    
}

#endif