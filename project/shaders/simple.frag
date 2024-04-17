#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragWorldPosition;
layout(location = 2) in vec3 fragWorldNormal;

layout(location = 0) out vec4 outColor;

layout (push_constant) uniform PushConstantData {
  mat4 modelMatrix; // projection * view * model
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
  vec3 diffuseLight = ubo.ambientLight.color.rgb * ubo.ambientLight.color.a;
  vec3 specularLight = vec3(0.0);
  vec3 surfaceNormal = normalize(fragWorldNormal);

  vec3 camWorldPos = ubo.inverseView[3].xyz;
  vec3 viewDirection = normalize(camWorldPos - fragWorldPosition);

  for (int i = 0; i < ubo.numPointLights; i++) {
    Light light = ubo.pointLights[i];
    // check if distance to light is within range
    if (light.range > 0.0 && length(light.position.xyz - fragWorldPosition) > light.range)
      continue;
    vec3 colorIntensity = light.color.rgb * light.color.a;

    vec3 lightDirection = light.position.xyz - fragWorldPosition;
    float attenuation = 1.0 / dot(lightDirection, lightDirection);
    lightDirection = normalize(lightDirection);

    float cosAngIncidence = max(dot(surfaceNormal, lightDirection), 0);
    vec3 intensity = colorIntensity * attenuation * cosAngIncidence;

    diffuseLight += intensity;

    // Specular
    vec3 halfAngle = normalize(lightDirection + viewDirection);
    float blinnTerm = clamp(dot(halfAngle, surfaceNormal), 0, 1);
    blinnTerm = pow(blinnTerm, 512.0);
    specularLight += colorIntensity * blinnTerm;
  }

  outColor = vec4((diffuseLight + specularLight) * fragColor, 1.0);
}
// 4 x 8 + 5 * 8 = (4 + 5) * 8