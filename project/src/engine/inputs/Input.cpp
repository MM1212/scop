#include "App.h"
#include "engine/input/Input.h"

namespace Scop::Input {
  static GLFWwindow* GetWindow() {
    return const_cast<GLFWwindow*>(App::Get().getWindow().getHandle());
  }
  bool IsKeyPressed(KeyCode key) {
    auto* window = GetWindow();
    auto state = glfwGetKey(window, static_cast<int>(key));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
  }
  bool IsKeyDown(KeyCode key) {
    auto* window = GetWindow();
    auto state = glfwGetKey(window, static_cast<int>(key));
    return state == GLFW_PRESS;
  }
  bool IsKeyUp(KeyCode key) {
    auto* window = GetWindow();
    auto state = glfwGetKey(window, static_cast<int>(key));
    return state == GLFW_RELEASE;
  }

  bool IsMouseButtonPressed(MouseCode button) {
    auto* window = GetWindow();
    auto state = glfwGetMouseButton(window, static_cast<int>(button));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
  }
  bool IsMouseButtonDown(MouseCode button) {
    auto* window = GetWindow();
    auto state = glfwGetMouseButton(window, static_cast<int>(button));
    return state == GLFW_PRESS;
  }
  bool IsMouseButtonUp(MouseCode button) {
    auto* window = GetWindow();
    auto state = glfwGetMouseButton(window, static_cast<int>(button));
    return state == GLFW_RELEASE;
  }
  glm::vec2 GetMousePosition() {
    double x, y;
    auto* window = GetWindow();
    glfwGetCursorPos(window, &x, &y);
    return { x, y };
  }
  float GetMouseX() {
    return GetMousePosition().x;
  }
  float GetMouseY() {
    return GetMousePosition().y;
  }
  void SetMouseMode(MouseMode mode) {
    auto* window = GetWindow();
    glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode));
  }
  MouseMode GetMouseMode() {
    auto* window = GetWindow();
    return static_cast<MouseMode>(glfwGetInputMode(window, GLFW_CURSOR));
  }
  void SetMousePosition(const glm::vec2& position) {
    auto* window = GetWindow();
    glfwSetCursorPos(window, position.x, position.y);
  }
}