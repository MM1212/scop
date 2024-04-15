#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <engine/entity/components/ID.h>
#include <engine/entity/components/Transform.h>


namespace Scop {
  class Entity {
  public:
    using ID = entt::entity;
    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    Entity(ID handle, entt::registry& reg) : handle(handle), registry{reg} {}
    ~Entity() = default;

    operator bool() const { return this->handle != entt::null; }
    operator ID() const { return this->handle; }
    bool operator==(const Entity& other) const { return this->handle == other.handle; }
    bool operator!=(const Entity& other) const { return this->handle != other.handle; }

    void setHandle(ID handle) { this->handle = handle; }

    template<typename Component, typename... Args>
    Component& addComponent(Args&&... args) {
      return this->registry.emplace<Component>(this->handle, std::forward<Args>(args)...);
    }

    template<typename Component>
    const Component& getComponent() const {
      return this->registry.get<Component>(this->handle);
    }

    template<typename Component>
    Component& getComponent() {
      return this->registry.get<Component>(this->handle);
    }

    template<typename ...Components>
    bool hasComponent() const {
      return this->registry.any_of<Components...>(this->handle);
    }

    template<typename Component, typename ...OtherComponents>
    bool removeComponent() {
      return this->registry.remove<Component, OtherComponents...>(this->handle) != 0;
    }

    // shortcuts
    entt::entity getHandle() const { return this->handle; }
    uint64_t getUniqId() const { return this->getComponent<Components::ID>().id; }
    const std::string_view getTag() const { return this->getComponent<Components::ID>().tag; }
    const auto& translation() const { return this->getComponent<Components::Transform>().translation; }
    auto& translation() { return this->getComponent<Components::Transform>().translation; }
    const auto& scale() const { return this->getComponent<Components::Transform>().scale; }
    auto& scale() { return this->getComponent<Components::Transform>().scale; }
    // TODO: when we go to 3d, make this return a ref
    auto rotation() const { return this->getComponent<Components::Transform>().rotation; }
    const Components::Transform& transform() const { return this->getComponent<Components::Transform>(); }
    Components::Transform& transform() { return this->getComponent<Components::Transform>(); }
    
  private:
    ID handle;
    entt::registry& registry;
  };
}