#include "App.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <stdexcept>
#include <array>
#include <chrono>

#include <engine/entity/components/Mesh.h>
#include <engine/entity/components/RigidBody2D.h>

using namespace Scop;

class GravityPhysicsSystem {
public:
  GravityPhysicsSystem(float strength) : strengthGravity{ strength } {}

  const float strengthGravity;

  // dt stands for delta time, and specifies the amount of time to advance the simulation
  // substeps is how many intervals to divide the forward time step in. More substeps result in a
  // more stable simulation, but takes longer to compute
  void update(std::vector<Entity>& entities, float dt, uint32_t substeps = 1) {
    const float stepDelta = dt / substeps;
    for (uint32_t i = 0; i < substeps; i++) {
      for (auto& objA : entities) {
        for (auto& objB : entities) {
          if (objA == objB) continue;
          auto& rigidBodyA = objA.getComponent<Components::RigidBody2D>();
          auto& rigidBodyB = objB.getComponent<Components::RigidBody2D>();
          auto force = computeForce(objA, objB);
          rigidBodyA.velocity += stepDelta * -force / rigidBodyA.mass;
          rigidBodyB.velocity += stepDelta * force / rigidBodyB.mass;
          // std::cout << "checking physics for "
          //   << objA.getTag() << "[" << static_cast<uint32_t>(objA.getHandle()) << "]"
          //   << " and "
          //   << objB.getTag() << "[" << static_cast<uint32_t>(objB.getHandle()) << "]"
          //   << " force: " << force.x << ", " << force.y
          //   << " A vel: " << rigidBodyA.velocity.x << ", " << rigidBodyA.velocity.y
          //   << " B vel: " << rigidBodyB.velocity.x << ", " << rigidBodyB.velocity.y
          //   << std::endl;
        }
      }

      // update each objects position based on its final velocity
      for (auto& entity : entities) {
        auto& rigidBody2D = entity.getComponent<Components::RigidBody2D>();
        entity.transform().translation += rigidBody2D.velocity * stepDelta;
      }
    }
  }

  glm::vec2 computeForce(Entity& fromObj, Entity& toObj) const {
    auto offset = fromObj.transform().translation - toObj.transform().translation;
    float distanceSquared = glm::dot(offset, offset);

    // clown town - just going to return 0 if objects are too close together...
    if (glm::abs(distanceSquared) < 1e-10f) {
      return { .0f, .0f };
    }
    // std::cout << "checking physics for "
    //   << fromObj.getTag() << "[" << static_cast<uint32_t>(fromObj.getHandle()) << "]"
    //   << " and "
    //   << toObj.getTag() << "[" << static_cast<uint32_t>(toObj.getHandle()) << "]"
    //   << std::endl;
    float toMass = toObj.hasComponent<Components::RigidBody2D>() ? toObj.getComponent<Components::RigidBody2D>().mass : 1.f;
    float fromMass = fromObj.hasComponent<Components::RigidBody2D>() ? fromObj.getComponent<Components::RigidBody2D>().mass : 1.f;
    float force = strengthGravity * toMass * fromMass / distanceSquared;
    return force * offset / glm::sqrt(distanceSquared);
  }
};

class Vec2FieldSystem {
public:
  void update(
    const GravityPhysicsSystem& physicsSystem,
    std::vector<Entity>& physicsObjs,
    std::vector<Entity>& vectorField) {
    // For each field line we caluclate the net graviation force for that point in space
    for (auto& vf : vectorField) {
      glm::vec2 direction{};
      for (auto& obj : physicsObjs) {
        direction += physicsSystem.computeForce(obj, vf);
      }

      // This scales the length of the field line based on the log of the length
      // values were chosen just through trial and error based on what i liked the look
      // of and then the field line is rotated to point in the direction of the field
      // std::cout << "direction: " << direction.x << ", " << direction.y << std::endl;
      auto& transform = vf.transform();
      transform.scale.x =
        0.005f + 0.045f * glm::clamp(glm::log(glm::length(direction) + 1) / 3.f, 0.f, 1.f);
      transform.rotation = atan2(direction.y, direction.x);
    }
  }
};

static std::unique_ptr<Renderer::Model> CreateSquareModel(Renderer::Device& device, glm::vec2 offset) {
  std::vector<Renderer::Model::Vertex> vertices = {
      {{-0.5f, -0.5f}},
      {{0.5f, 0.5f}},
      {{-0.5f, 0.5f}},
      {{-0.5f, -0.5f}},
      {{0.5f, -0.5f}},
      {{0.5f, 0.5f}},  //
  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<Renderer::Model>(device, vertices);
}

static std::unique_ptr<Renderer::Model> CreateCircleModel(Renderer::Device& device, uint32_t numSides) {
  std::vector<Renderer::Model::Vertex> uniqueVertices{};
  for (uint32_t i = 0; i < numSides; i++) {
    float angle = i * glm::two_pi<float>() / numSides;
    uniqueVertices.push_back({ {glm::cos(angle), glm::sin(angle)} });
  }
  uniqueVertices.push_back({});  // adds center vertex at 0, 0

  std::vector<Renderer::Model::Vertex> vertices{};
  for (uint32_t i = 0; i < numSides; i++) {
    vertices.push_back(uniqueVertices[i]);
    vertices.push_back(uniqueVertices[(i + 1) % numSides]);
    vertices.push_back(uniqueVertices[numSides]);
  }
  return std::make_unique<Renderer::Model>(device, vertices);
}


App::App() {
  this->loadEntities();
}

App::~App() {}

void App::run() {
  std::shared_ptr<Renderer::Model> squareModel = CreateSquareModel(this->device, { 0.5f, 0.0f });
  std::shared_ptr<Renderer::Model> circleModel = CreateCircleModel(this->device, 64);

  // create vector field
  int gridCount = 40;
  std::vector<Entity> vectorField;
  for (int i = 0; i < gridCount; i++) {
    for (int j = 0; j < gridCount; j++) {
      auto vf = this->scene.createEntity("VectorField");
      vf.transform().scale = glm::vec2(0.005f);
      vf.transform().translation = {
          -1.0f + (i + 0.5f) * 2.0f / gridCount,
          -1.0f + (j + 0.5f) * 2.0f / gridCount };
      vf.addComponent<Components::Mesh>(squareModel, glm::vec3{ 1.f });
      vectorField.push_back(std::move(vf));
    }
  }

  // create physics objects
  std::vector<Entity> physicsObjects{};
  auto red = this->scene.createEntity("Red");
  std::cout << "red Entity ID: " << static_cast<uint32_t>(red.getHandle()) << std::endl;
  red.transform().scale = glm::vec2{ .05f };
  red.transform().translation = { .5f, .5f };
  red.addComponent<Components::Mesh>(circleModel, glm::vec3{ 1.f, 0.f, 0.f });
  red.addComponent<Components::RigidBody2D>(1.f, glm::vec2{ -.5f, .0f });
  physicsObjects.push_back(std::move(red));
  auto blue = this->scene.createEntity("Blue");
  std::cout << "blue Entity ID: " << static_cast<uint32_t>(red.getHandle()) << std::endl;
  blue.transform().scale = glm::vec2{ .05f };
  blue.transform().translation = { -.45f, -.25f };
  blue.addComponent<Components::Mesh>(circleModel, glm::vec3{ 0.f, 0.f, 1.f });
  blue.addComponent<Components::RigidBody2D>(1.f, glm::vec2{ .5f, .0f });
  physicsObjects.push_back(std::move(blue));



  GravityPhysicsSystem gravitySystem{ 0.81f };
  Vec2FieldSystem vecFieldSystem{};

  Renderer::Systems::Simple simpleRenderSystem{ this->device, this->renderer.getSwapchainRenderPass() };
  while (!this->window.shouldClose()) {
    this->window.pollEvents();
    auto cmdBuffer = this->renderer.beginFrame();
    if (!cmdBuffer)
      continue;
    // update the physics system
    (void)gravitySystem;
    (void)vecFieldSystem;
    gravitySystem.update(physicsObjects, 1.f / 60, 5);
    vecFieldSystem.update(gravitySystem, physicsObjects, vectorField);

    // render
    this->renderer.beginSwapchainRenderPass(cmdBuffer);
    simpleRenderSystem.renderScene(cmdBuffer, this->scene);
    this->renderer.endSwapchainRenderPass(cmdBuffer);
    this->renderer.endFrame();
    vkDeviceWaitIdle(this->device.getHandle());
  }
  vkDeviceWaitIdle(this->device.getHandle());
}

void App::loadEntities() {
  // std::vector<Renderer::Model::Vertex> vertices{
  //   {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //   {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //   {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  // };
  // auto model = std::make_shared<Renderer::Model>(this->device, vertices);

  // std::vector<glm::vec3> colors{
  //   {1.f, .7f, .73f},
  //   {1.f, .87f, .73f},
  //   {1.f, 1.f, .73f},
  //   {.73f, 1.f, .8f},
  //   {.73, .88f, 1.f}
  // };
  // for (auto& color : colors) {
  //   color = glm::pow(color, glm::vec3{ 2.2f });
  // }
  // for (int i = 0; i < 40; i++) {
  //   auto triangle = this->scene.createEntity("Triangle" + std::to_string(i));
  //   triangle.addComponent<Components::Mesh>(model, colors[i % colors.size()]);
  //   auto& transform = triangle.transform();
  //   transform.scale = glm::vec2(.5f) + i * 0.025f;
  //   transform.rotation = i * glm::pi<float>() * .025f;
  // }
}

