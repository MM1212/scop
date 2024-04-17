#pragma once

#include "KeyCodes.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Scop::Input {
  bool IsKeyPressed(KeyCode key);
  bool IsKeyDown(KeyCode key);
  bool IsKeyUp(KeyCode key);

  enum class MouseMode {
    Normal = GLFW_CURSOR_NORMAL,
    Hidden = GLFW_CURSOR_HIDDEN,
    Disabled = GLFW_CURSOR_DISABLED
  };
  bool IsMouseButtonPressed(MouseCode button);
  bool IsMouseButtonDown(MouseCode button);
  bool IsMouseButtonUp(MouseCode button);
  glm::vec2 GetMousePosition();
  float GetMouseX();
  float GetMouseY();
  glm::vec2 GetMouseDelta();
  void SetMouseMode(MouseMode mode);
  MouseMode GetMouseMode();
  void SetMousePosition(const glm::vec2& position);
  void Update();
  void Init(GLFWwindow* window);
}