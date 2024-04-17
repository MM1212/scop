#include "InputManager.h"

namespace Scop::Input {
  class InputManager_Internal {
  public:
    static InputManager* Get() {
      return InputManager::Get();
    }
    static void HandleKeyInput(uint16_t key, int action);
    static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GlfwCursorPosCallback(GLFWwindow* window, double x, double y);
    static void GlfwScrollCallback(GLFWwindow* window, double x, double y);
    static void Update();
    static void BindCallbacks(GLFWwindow* window);
  };
}
