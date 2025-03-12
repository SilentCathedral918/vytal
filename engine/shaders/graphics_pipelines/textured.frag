#version 450

layout(binding = 1) uniform sampler2D texture_sampler;

layout(location = 0) in vec2 frag_texture_uv;

layout(location = 0) out vec4 out_colour;

void main() {
    out_colour = texture(texture_sampler, frag_texture_uv);
}