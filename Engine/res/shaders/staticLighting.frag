#version 420

#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 in_fragPos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in mat4 in_view;

layout(location = 0) out vec4 out_fragColor;

layout(location = 3) uniform vec3 u_lightPos = vec3(2.0, 2.0, 0.0);
layout(location = 4) uniform vec3 u_lightColor = vec3(1.0);

layout(location = 5) uniform vec3 u_objectColor = vec3(0.8, 0.8, 0.2);

void main() 
{
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_lightColor;

	vec3 norm = normalize(in_normal);
	vec3 lightDir = normalize(vec3(in_view * vec4(u_lightPos, 1.0)) - in_fragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_lightColor;

	float specularStrength = 0.0;

	vec3 viewDir = normalize(-in_fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(norm, halfwayDir), 0.0), 32);
	vec3 specular = specularStrength * spec * u_lightColor;

	vec3 result = (ambient + diffuse + specular) * u_objectColor;
    out_fragColor = vec4(result, 1.0);
}