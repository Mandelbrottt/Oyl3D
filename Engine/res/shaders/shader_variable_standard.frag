#version 420

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoords;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec4 out_color;

uniform sampler2D u_texture;

void main()
{
    out_color = vec4(1.0);
}
