#version 420

#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_texCoord;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec3 out_fragPos;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out mat4 out_view;

layout(location = 0) uniform mat4 u_model = mat4(1.0);
layout(location = 1) uniform mat4 u_view;
layout(location = 2) uniform mat4 u_projection;

void main() 
{
	mat4 modelView = u_view * u_model;

	gl_Position = u_projection * modelView * vec4(in_position, 1.0);

	out_fragPos = vec3(modelView * vec4(in_position, 1.0));

	out_normal = mat3(transpose(inverse(modelView))) * in_normal;

	out_view = u_view;
}