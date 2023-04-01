#ifndef SVE_WINDOW_INCLUDED
#define SVE_WINDOW_INCLUDED

// libs
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>


namespace sve {
    class Window {
        public:
        Window(int w, int h, std::string title);
        ~Window();

        bool shouldClose() { return glfwWindowShouldClose(window); }
        bool wasWindowResized() { return frameBufferResized; }
        void resetWindowResizedFlag() { frameBufferResized = false; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        GLFWwindow* getGLFWwindow() const { return window; }
        protected:

        void initWindow();  

        private:
        static void frameBufferResizedCallback(GLFWwindow* window, int width, int height);


        int width, height;
        std::string title;
        bool frameBufferResized = false;

        GLFWwindow* window;
    };
};

#endif