#version 420

struct Material 
{
	sampler2D albedo;
	sampler2D specular;
	sampler2D normal;
	float specularScalar; // temporary
};

struct PointLight 
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirectionalLight 
{
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight 
{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoords;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec4 out_color;

uniform Material u_material;

uniform PointLight u_pointLight;

void main() 
{
	vec3 norm = normalize(in_normal);
	vec3 lightDir = normalize(u_pointLight.position - in_position);

	float diff = max(dot(norm, lightDir), 0.0);

	vec3 viewDir = normalize(-in_position);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(norm, halfwayDir), 0.0), 32);
	
    vec3 ambient  = u_pointLight.ambient  *        vec3(texture(u_material.albedo,   in_texCoords));
	vec3 diffuse  = u_pointLight.diffuse  * diff * vec3(texture(u_material.albedo,   in_texCoords));
	vec3 specular = u_pointLight.specular * spec * vec3(texture(u_material.specular, in_texCoords));
	// vec3 specular = u_pointLight.specular * spec * u_material.specularScalar; // temporary

	out_color = vec4((ambient + diffuse + specular), 1.0);
}