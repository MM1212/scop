#pragma once

#include <glm/glm.hpp>
#include <engine/renderer/Model.h>
#include <memory>

namespace Scop::Components {
  struct Mesh {  
    Mesh() = default;
    Mesh(const Mesh&) = default;
    Mesh& operator=(const Mesh&) = default;
    Mesh(std::shared_ptr<Renderer::Model> model) : model(model) {}
    Mesh(std::unique_ptr<Renderer::Model>& model) : model(std::move(model)) {}
    Mesh(std::shared_ptr<Renderer::Model> model, const glm::vec3& color) : model(model), color(color) {}

    std::shared_ptr<Renderer::Model> model;
    glm::vec3 color{ 1.0f, 1.0f, 1.0f };
  };
}