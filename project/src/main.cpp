#include <scop.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main() {

  std::cout << "Hello, World!" << std::endl;
  std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
  bool vulkanSupported = glfwVulkanSupported();
  std::cout << "Vulkan supported: " << vulkanSupported << std::endl;
  uint32_t count;
  const char** extensions = glfwGetRequiredInstanceExtensions(&count);
  std::cout << "Required extensions: " << count << std::endl;
  for (uint32_t i = 0; i < count; i++) {
    std::cout << extensions[i] << std::endl;
  }
  return 0;
}