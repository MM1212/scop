#include "systems/Profiler.hpp"

#include <engine/input/Input.h>

#include <iostream>

using Scop::Profiler;

void Profiler::update(float deltaTime) {
  if (Input::IsKeyDown(Input::Key::P))
    this->enabled = !this->enabled;
  if (!this->enabled)
    return;
  this->lastOutput -= deltaTime;
  if (this->lastOutput <= 0.f) {
    this->lastOutput = 1.f;
    std::cout << "Profiler: " << deltaTime * 1000 << "ms, fps: " << 1.f / deltaTime << std::endl;
  }
}