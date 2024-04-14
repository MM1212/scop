#pragma once

#include <entt/entt.hpp>
#include <engine/entity/Entity.h>
#include <utils/Random.h>

namespace Scop {
  class Scene {
  public:
    Scene() : id(Utils::Random::Int<uint64_t>()) {}
    ~Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Entity createEntity(const std::string_view tag = "");
    void destroyEntity(Entity entity);

    entt::registry& getRegistry() { return this->registry; }
    const entt::registry& getRegistry() const { return this->registry; }

    template<typename ...Components>
    auto groupEntitiesWith() {
      return this->registry.group<Components...>();
    }

    template<typename ...Components>
    auto viewEntitiesWith() {
      return this->registry.view<Components...>();
    }
  private:
    uint64_t id = 0;
    entt::registry registry;
  };
}