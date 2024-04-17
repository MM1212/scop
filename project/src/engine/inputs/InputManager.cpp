#include "engine/input/InputManager_Internal.h"

using namespace Scop::Input;

void InputManager_Internal::HandleKeyInput(uint16_t key, int action) {
  static auto inputManager = InputManager::Get();
  auto& keys = inputManager->keys;
  auto& toUpdate = inputManager->toUpdate;

  if (action == GLFW_PRESS) {
    keys[static_cast<KeyCode>(key)].pressed = true;
    keys[static_cast<KeyCode>(key)].justPressed = true;
    toUpdate.insert(static_cast<KeyCode>(key));
  }
  else if (action == GLFW_RELEASE) {
    keys[static_cast<MouseCode>(key)].pressed = false;
    keys[static_cast<MouseCode>(key)].justReleased = true;
    toUpdate.insert(static_cast<MouseCode>(key));
  }
}

void InputManager_Internal::GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;
  (void)window;
  HandleKeyInput(key, action);
}

void InputManager_Internal::GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  (void)mods;
  (void)window;
  HandleKeyInput(button, action);
}

void InputManager_Internal::GlfwCursorPosCallback(GLFWwindow* window, double x, double y) {
  (void)window;
  static auto inputManager = InputManager::Get();
  inputManager->lastMousePosition = inputManager->mousePosition;
  inputManager->mousePosition = { x, y };
  inputManager->mouseDelta = inputManager->mousePosition - inputManager->lastMousePosition;
}

void InputManager_Internal::GlfwScrollCallback(GLFWwindow* window, double x, double y) {
  (void)window;
  static auto inputManager = InputManager::Get();
  inputManager->scrollOffset = { x, y };
}

void InputManager_Internal::Update() {
  static auto inputManager = InputManager::Get();
  auto& keys = inputManager->keys;
  auto& toUpdate = inputManager->toUpdate;

  for (auto& key : toUpdate) {
    keys[key].justPressed = false;
    keys[key].justReleased = false;
  }
  toUpdate.clear();
}

void InputManager_Internal::BindCallbacks(GLFWwindow* window) {
  glfwSetKeyCallback(window, GlfwKeyCallback);
  glfwSetMouseButtonCallback(window, GlfwMouseButtonCallback);
  glfwSetCursorPosCallback(window, GlfwCursorPosCallback);
  glfwSetScrollCallback(window, GlfwScrollCallback);
}

const InputManager::KeyState* InputManager::GetKeyState(KeyCode key) const {
  auto it = keys.find(key);
  if (it == keys.end())
    return nullptr;
  return &it->second;
}

void InputManager::SetMouseMode(MouseMode mode) {
  auto window = const_cast<GLFWwindow*>(App::Get().getWindow().getHandle());
  glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode));
  this->mouseMode = mode;
}
void InputManager::SetMousePosition(glm::vec2 position) {
  auto window = const_cast<GLFWwindow*>(App::Get().getWindow().getHandle());
  glfwSetCursorPos(window, position.x, position.y);
  this->mousePosition = position;
}