#include "engine/Scene.h"

using Scop::Scene;
using Scop::Entity;

Entity Scene::createEntity(const std::string_view tag) {
  Entity entity{ this->registry.create(), this->registry };
  uint64_t id = Components::ID::GenerateId();
  if (tag.empty())
    entity.addComponent<Components::ID>("Entity " + std::to_string(id), id);
  else
    entity.addComponent<Components::ID>(tag, id);
  entity.addComponent<Components::Transform>();
  return entity;
}
void Scene::destroyEntity(Entity entity) {
  this->registry.destroy(entity);
}
