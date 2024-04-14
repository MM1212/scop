#include "engine/entity/components/ID.h"

#include <random>

using Scop::Components::ID;

uint64_t ID::NewRandomId() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());
  return dis(gen);
}