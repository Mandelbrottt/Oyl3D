#version 420

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec2 out_texCoords;

uniform mat4 u_model;

uniform mat4 u_viewProjection;

void main() 
{
	vec4 modelPosition = u_model * vec4(in_position, 1.0);

    vec4 outPosition = u_viewProjection * modelPosition;

	gl_Position = outPosition;

    out_texCoords = (outPosition.xy / outPosition.w) * 0.5 + 0.5;
}