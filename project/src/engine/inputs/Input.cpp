#include "App.h"
#include "engine/input/Input.h"
#include "engine/input/InputManager_Internal.h"

namespace Scop::Input {
  void Update() {
    InputManager_Internal::Update();
  }
  void Init(GLFWwindow* window) {
    InputManager_Internal::BindCallbacks(window);
  }
  static inline InputManager* GetManager() {
    static auto manager = InputManager::Get();
    return manager;
  }
  bool IsKeyPressed(KeyCode key) {
    return GetManager()->IsKeyPressed(key);
  }
  bool IsKeyDown(KeyCode key) {
    return GetManager()->IsKeyJustPressed(key);
  }
  bool IsKeyUp(KeyCode key) {
    return GetManager()->IsKeyJustReleased(key);
  }

  bool IsMouseButtonPressed(MouseCode button) {
    return GetManager()->IsKeyPressed(button);
  }
  bool IsMouseButtonDown(MouseCode button) {
    return GetManager()->IsKeyJustPressed(button);
  }
  bool IsMouseButtonUp(MouseCode button) {
    return GetManager()->IsKeyJustReleased(button);
  }
  glm::vec2 GetMousePosition() {
    return GetManager()->GetMousePosition();
  }
  float GetMouseX() {
    return GetMousePosition().x;
  }
  float GetMouseY() {
    return GetMousePosition().y;
  }

  glm::vec2 GetMouseDelta() {
    return GetManager()->GetMouseDelta();
  }
  void SetMouseMode(MouseMode mode) {
    GetManager()->SetMouseMode(mode);
  }
  MouseMode GetMouseMode() {
    return GetManager()->GetMouseMode();
  }
  void SetMousePosition(const glm::vec2& position) {
    GetManager()->SetMousePosition(position);
  }
}