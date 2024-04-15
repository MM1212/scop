#pragma once

#include <GLFW/glfw3.h>
#include <string_view>
#include <glm/glm.hpp>

namespace Scop {
  class Window {
  public:
    Window(const uint32_t width, const uint32_t height, const std::string_view title);
    Window(const glm::uvec2 size, const std::string_view title);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    const GLFWwindow* getHandle() const { return this->handle; }
    GLFWwindow* getHandle() { return this->handle; }
    const glm::uvec2& getSize() const { return this->size; }
    VkExtent2D getExtent() const { return { this->size.x, this->size.y }; }
    uint32_t getWidth() const { return this->size.x; }
    uint32_t getHeight() const { return this->size.y; }
    bool shouldClose() const { return glfwWindowShouldClose(this->handle); }
    void close() { glfwSetWindowShouldClose(this->handle, GLFW_TRUE); }
    void pollEvents() { glfwPollEvents(); }
    bool wasResized() const { return this->resized; }
    void resetResizedFlag() { this->resized = false; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
  private:
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    void init();

    glm::uvec2 size;
    bool resized = false;
    const std::string_view title;
    GLFWwindow* handle;
  };
}