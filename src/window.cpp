#include "window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

// std
#include <stdexcept>

namespace sve
{

    Window::Window(int w, int h, std::string title) 
        : width{w}, height{h}, title{title}
    {
        initWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("[sve] failed to intialize windows surface.");
        }
    }

    void Window::frameBufferResizedCallback(GLFWwindow* window, int width, int height)
    {
        auto sveWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        sveWindow->frameBufferResized = true;
        sveWindow->width = width;
        sveWindow->height = height;
    }

}