#pragma once

#include <glm/glm.hpp>
namespace Scop::Utils {
  glm::vec3   Rgb2hsv(glm::vec3 in);
  glm::vec3   Hsv2rgb(glm::vec3 in);
}