#pragma once

#include "Input.h"
#include <utils/Singleton.h>
#include <App.h>

#include <array>
#include <unordered_set>
#include <glm/glm.hpp>

namespace Scop::Input {
  class InputManager_Internal;
  class InputManager : public Singleton<InputManager> {
  private:
    static constexpr uint32_t MAX_KEYS = GLFW_KEY_LAST;
  public:
    InputManager() : toUpdate(MAX_KEYS) {}
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    bool IsKeyPressed(KeyCode key) const {
      auto state = this->GetKeyState(key);
      return state && state->pressed;
    }
    bool IsKeyReleased(KeyCode key) const {
      return !this->IsKeyPressed(key);
    }
    bool IsKeyJustPressed(KeyCode key) const {
      auto state = this->GetKeyState(key);
      return state && !state->wasPressed && state->pressed;
    }
    bool IsKeyJustReleased(KeyCode key) const {
      auto state = this->GetKeyState(key);
      return state && state->wasPressed && !state->pressed;
    }
    bool IsKeyHeld(KeyCode key) const {
      auto state = this->GetKeyState(key);
      return state && state->wasPressed && state->pressed;
    }
    glm::vec2 GetMousePosition() const {
      return this->mousePosition;
    }
    float GetMouseX() const {
      return this->mousePosition.x;
    }
    float GetMouseY() const {
      return this->mousePosition.y;
    }
    glm::vec2 GetMouseDelta() const {
      return this->mouseDelta;
    }
    glm::vec2 GetScrollOffset() const {
      return this->scrollOffset;
    }
    MouseMode GetMouseMode() const {
      return this->mouseMode;
    }
    void SetMouseMode(MouseMode mode);
    void SetMousePosition(glm::vec2 position);
    float GetScrollY() const {
      return this->scrollOffset.y;
    }
    float GetScrollX() const {
      return this->scrollOffset.x;
    }
  private:
    struct KeyState {
      bool pressed = false;
      bool wasPressed = false;
    };
    const KeyState* GetKeyState(KeyCode key) const;
    KeyState* GetKeyState(KeyCode key);

    std::array<KeyState, GLFW_KEY_LAST> keys{};
    std::unordered_set<KeyCode> toUpdate;
    glm::vec2 mousePosition = { 0.0f, 0.0f };
    glm::vec2 mouseDelta = { 0.0f, 0.0f };
    glm::vec2 lastMousePosition = { 0.0f, 0.0f };
    glm::vec2 scrollOffset = { 0.0f, 0.0f };
    MouseMode mouseMode = MouseMode::Normal;

    friend class Singleton<InputManager>;
    friend class InputManager_Internal;
    friend class Input;
  };
};