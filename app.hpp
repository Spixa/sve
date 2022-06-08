#ifndef SVE_APP_INCLUDED
#define SVE_APP_INCLUDED
#include "window.hpp"
#include "renderer.hpp"
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

    class App {
    public:
    
    static constexpr int WIDTH = 800, HEIGHT = 600;

    App();
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;


    void run();
    private:
    void loadGameObjects();

    sve::Window window{WIDTH, HEIGHT, "SVE"};

    Device device{window};

    sve::Renderer renderer{window, device};
    std::vector<sve::GameObject> gameObjects;

};

    
}

#endif