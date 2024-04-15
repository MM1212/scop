#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;


layout (location = 0) out vec3 fragColor;

layout (push_constant) uniform PushConstantData {
  mat4 modelMatrix; // projection * view * model
  mat4 normalMatrix; // model
} pushData;

layout (set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionView;
  vec3 globalLightDirection;
  vec3 globalLightColor;
} ubo;


const float AMBIENT_INTENSITY = 0.02;

void main() {
  gl_Position = ubo.projectionView * pushData.modelMatrix * vec4(position, 1.0);

  // vec3 normalWorldSpace = normalize(mat3(pushData.modelMatrix) * normal);
  // mat3 normalMatrix = transpose(inverse(mat3(pushData.modelMatrix)));
  vec3 normalWorldSpace = normalize(mat3(pushData.normalMatrix) * normal);

  float lightIntensity = AMBIENT_INTENSITY + max(dot(normalWorldSpace, ubo.globalLightDirection), 0.0);

  fragColor = color * (lightIntensity * ubo.globalLightColor);
}