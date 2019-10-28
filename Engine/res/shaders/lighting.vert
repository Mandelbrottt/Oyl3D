#version 420

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_texCoord;
layout(location = 2) out vec3 out_normal;

uniform mat4 u_model;

uniform mat4 u_view;
uniform mat4 u_viewProjection;
uniform mat3 u_viewNormal;

void main() 
{
	gl_Position = u_viewProjection * u_model * vec4(in_position, 1.0);

	out_position = vec3(u_view * u_model * vec4(in_position, 1.0));

	out_texCoord = in_texCoord;

	out_normal = u_viewNormal * mat3(u_model) * in_normal;
}