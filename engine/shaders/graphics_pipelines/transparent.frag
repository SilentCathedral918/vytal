#version 450

layout(binding = 1) uniform sampler2D texture_sampler;

layout(location = 0) in vec2 frag_texture_uv;

layout(location = 0) out vec4 out_color;

void main() {
    vec4 tex_color = texture(texture_sampler, frag_texture_uv);

    if (tex_color.a < 0.1) 
        discard;

    out_color = tex_color;
}
