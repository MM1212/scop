#version 450

layout(location = 0) in vec2 fragOffset;
layout(location = 0) out vec4 outColor;

layout (push_constant) uniform PushConstantData {
  vec4 position;
  vec4 color;
  float width;
  float height;
  bool rounded;
} pushData;

# define MAX_LIGHTS 16
struct Light {
  vec4 color;
  float range; // unused
  vec4 position;
};

layout (set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 projectionView;
  mat4 inverseView;
  Light ambientLight;
  Light pointLights[MAX_LIGHTS];
  int numPointLights;
} ubo;

const float M_PI = 3.14159265359;

void main() {
    float distance = sqrt(dot(fragOffset, fragOffset));
  if (pushData.rounded && distance >= 1.0)
      discard;
  float cosDistance =  0.5 * (cos(distance * M_PI) + 1.0);
  outColor = vec4(pushData.color.rgb + cosDistance, cosDistance);
}