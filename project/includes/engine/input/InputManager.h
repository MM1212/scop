#pragma once

#include "Input.h"
#include <utils/Singleton.h>
#include <App.h>

#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>

namespace Scop::Input {
  class InputManager_Internal;
  class InputManager : public Singleton<InputManager> {
  public:
    InputManager() = default;
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
      return state && state->justPressed;
    }
    bool IsKeyJustReleased(KeyCode key) const {
      auto state = this->GetKeyState(key);
      return state && state->justReleased;
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
      bool justPressed = false;
      bool justReleased = false;
    };
    const KeyState* GetKeyState(KeyCode key) const;

    std::unordered_map<KeyCode, KeyState> keys;
    std::unordered_set<KeyCode> toUpdate;
    glm::vec2 mousePosition;
    glm::vec2 mouseDelta;
    glm::vec2 lastMousePosition;
    glm::vec2 scrollOffset;
    MouseMode mouseMode = MouseMode::Normal;

    friend class Singleton<InputManager>;
    friend class InputManager_Internal;
    friend class Input;
  };
};