#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

layout(location = 0) out vec3 frag_colour;

vec3 random_colour(vec3 seed) {
    vec3 result = vec3(
        fract(sin(dot(seed, vec3(12.9898, 78.233, 45.543))) * 43758.5453),
        fract(sin(dot(seed, vec3(93.9898, 67.345, 19.234))) * 43758.5453),
        fract(sin(dot(seed, vec3(41.1231, 94.684, 77.891))) * 43758.5453)
    );

    return result;
}

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
    frag_colour = random_colour(in_position);
}
