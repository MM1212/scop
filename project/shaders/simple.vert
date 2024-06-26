#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;


layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragWorldPosition;
layout (location = 2) out vec3 fragWorldNormal;

layout (push_constant) uniform PushConstantData {
  mat4 modelMatrix; // model
  mat4 normalMatrix; // model
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


void main() {
  vec4 worldPosition = pushData.modelMatrix * vec4(position, 1.0);
  gl_Position = ubo.projectionView * pushData.modelMatrix * vec4(position, 1.0);

  // vec3 normalWorldSpace = normalize(mat3(pushData.modelMatrix) * normal);
  // mat3 normalMatrix = transpose(inverse(mat3(pushData.modelMatrix)));
  fragWorldNormal = normalize(mat3(pushData.normalMatrix) * normal);
  fragWorldPosition = worldPosition.xyz;
  fragColor = color;
}