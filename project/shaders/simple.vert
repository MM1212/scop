#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 fragColor;

layout (push_constant) uniform PushConstantData {
  mat4 transform;
  vec3 color;
} pushData;

void main() {
  gl_Position = pushData.transform * vec4(position, 1.0);
  fragColor = color;
}