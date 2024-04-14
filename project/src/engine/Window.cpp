#include "engine/Window.h"
#include <stdexcept>

using Scop::Window;

Window::Window(
  const uint32_t width,
  const uint32_t height,
  const std::string_view title
) : size(width, height), title(title), handle(nullptr) {
  this->init();
}
Window::Window(
  const glm::uvec2 size,
  const std::string_view title
) : size(size), title(title), handle(nullptr) {
  this->init();
}
Window::~Window() {
  if (!this->handle)
    return;
  glfwDestroyWindow(this->handle);
  glfwTerminate();
}

void Window::init() {
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  this->handle = glfwCreateWindow(
    this->size.x, this->size.y,
    this->title.data(), nullptr, nullptr
  );
  if (!this->handle)
    throw std::runtime_error("Failed to create window");
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
  if (glfwCreateWindowSurface(instance, this->handle, nullptr, surface) != VK_SUCCESS)
    throw std::runtime_error("Failed to create window surface");
}
