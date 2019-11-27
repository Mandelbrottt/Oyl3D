#version 420

layout(location = 0) in vec3 in_position_curr;
layout(location = 1) in vec2 in_texCoord_curr;
layout(location = 2) in vec3 in_normal_curr;

layout(location = 3) in vec3 in_position_next;
layout(location = 4) in vec2 in_texCoord_next;
layout(location = 5) in vec3 in_normal_next;

layout(location = 6) in vec3 in_position_trans;
layout(location = 7) in vec2 in_texCoord_trans;
layout(location = 8) in vec3 in_normal_trans;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_texCoord;
layout(location = 2) out vec3 out_normal;

uniform mat4 u_model;

uniform mat4 u_view;
uniform mat4 u_viewProjection;
uniform mat3 u_viewNormal;

uniform float lerpT_curr;
uniform float lerpT_trans;

void main() 
{
	vec3 finalPos = mix(in_position_curr, in_position_next, lerpT_curr);
	finalPos      = mix(finalPos, in_position_trans, lerpT_trans); 
	gl_Position   = u_viewProjection * u_model * vec4(finalPos, 1.0);

	out_position = vec3(u_view * u_model * vec4(finalPos, 1.0));

	vec2 finalTex = mix(in_texCoord_curr, in_texCoord_next, lerpT_curr); 
	out_texCoord  = mix(finalTex, in_texCoord_trans, lerpT_trans);

	vec3 finalNormal = mix(in_normal_curr, in_normal_next, lerpT_curr);
	out_normal = u_viewNormal * mat3(u_model) * mix(finalNormal, in_normal_trans, lerpT_trans);
}