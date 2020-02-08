#version 420

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;

// layout(location = 0) out vec3 out_position;
// layout(location = 1) out vec2 out_texCoord;
// layout(location = 2) out vec3 out_normal;

out VS_OUT {
	vec3 position;
	vec2 texCoord;
	mat3 TBN;
	mat3 viewNormal;
	vec4 lightSpacePosition;
} vs_out;

uniform mat4 u_model;

uniform mat4 u_view;
uniform mat4 u_viewProjection;
uniform mat3 u_viewNormal;
uniform mat4 u_lightSpaceMatrix;

void main() 
{
	vec4 modelPosition = u_model * vec4(in_position, 1.0);

	gl_Position = u_viewProjection * modelPosition;

	vs_out.position = vec3(u_view * modelPosition);
	vs_out.texCoord = in_texCoord;

	vs_out.lightSpacePosition = u_lightSpaceMatrix * modelPosition;

	mat3 viewNormalModel = u_viewNormal * mat3(u_model);

	vec3 T = normalize(viewNormalModel * in_tangent);
	vec3 B = normalize(viewNormalModel * in_bitangent);
	vec3 N = normalize(viewNormalModel * in_normal);

	vs_out.TBN = mat3(T, B, N);

	vs_out.viewNormal = u_viewNormal;

	// out_normal = u_viewNormal * mat3(u_model) * in_normal;
}