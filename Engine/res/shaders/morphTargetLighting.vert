#version 420

layout(location = 0) in vec3 in_position1;
layout(location = 1) in vec2 in_texCoord1;
layout(location = 2) in vec3 in_normal1;

layout(location = 3) in vec3 in_position2;
layout(location = 4) in vec2 in_texCoord2;
layout(location = 5) in vec3 in_normal2;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_texCoord;
layout(location = 2) out vec3 out_normal;

uniform mat4 u_model;

uniform mat4 u_view;
uniform mat4 u_viewProjection;
uniform mat3 u_viewNormal;

uniform float lerpT;

void main() 
{
	gl_Position = u_viewProjection * u_model * vec4(mix(in_position1, in_position2, lerpT), 1.0);

	out_position = vec3(u_view * u_model * vec4(mix(in_position1, in_position2, lerpT), 1.0));

	out_texCoord = mix(in_texCoord1, in_texCoord2, lerpT);

	out_normal = u_viewNormal * mat3(u_model) * mix(in_normal1, in_normal2, lerpT);
	
}