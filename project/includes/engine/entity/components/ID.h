#pragma once

#include <string>

namespace Scop::Components {
  struct ID {
    std::string tag;
    uint64_t id;

    bool operator==(const ID& other) const {
      return this->id == other.id;
    }
    bool operator!=(const ID& other) const {
      return this->id != other.id;
    }

    ID() = default;
    ID(const ID&) = default;
    ID& operator=(const ID&) = default;
    ID(const std::string_view tag, const uint64_t id) : tag(tag), id(id) {}
    static uint64_t NewRandomId();
  };
}