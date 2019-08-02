#version 420 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_textureUV;
layout(location = 2) in vec3 a_normal;

out vec2 v_texCoord;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
	v_texCoord = a_textureUV;
	v_normal = mat3(u_view) * mat3(u_model) * a_normal;
}