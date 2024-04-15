#pragma once

#include <entt/entt.hpp>
#include <utils/Random.h>
#include <engine/scene/components/all.h>
#include <unordered_map>
#include <engine/scene/SceneCamera.h>

namespace Scop {
  class Entity;
  class Scene {
  public:
    Scene() : id(Utils::Random::Int<uint64_t>()) {}
    ~Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Entity createEntity(const std::string_view tag = "");
    void destroyEntity(Entity entity);
    bool entityExists(entt::entity id) const;
    bool entityExists(uint64_t uuid) const;

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
    std::unordered_map<uint64_t, entt::entity> entityMap;

    friend class Entity;
  };
}