#version 450

layout (push_constant) uniform PushConstantData {
  vec4 position;
  vec4 color;
  float width;
  float height;
  bool rounded;
} pushData;

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragOffset;

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
  Light ambientLight;
  Light pointLights[MAX_LIGHTS];
  int numPointLights;
} ubo;

void main() {
  fragOffset = OFFSETS[gl_VertexIndex];
  vec3 camWorldRight = {ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]};
  vec3 camWorldUp = {ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]};

  vec4 worldPosition = vec4(pushData.position.xyz
    + pushData.width * fragOffset.x * camWorldRight
    + pushData.height * fragOffset.y * camWorldUp, 1.0);

  gl_Position = ubo.projectionView * worldPosition;

}