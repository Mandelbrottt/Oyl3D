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

uniform PointLight u_pointLight[2];

void main() 
{
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	for (int i = 0; i < 8; i++) 
	{
		vec3 norm = normalize(in_normal);
		vec3 lightDir = normalize(u_pointLight[i].position - in_position);

		float diff = max(dot(norm, lightDir), 0.0);

		vec3 viewDir = normalize(-in_position);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		float spec = pow(max(dot(norm, halfwayDir), 0.0), 32);

    	ambient  += u_pointLight[i].ambient  *        vec3(texture(u_material.albedo,   in_texCoords));
		diffuse  += u_pointLight[i].diffuse  * diff * vec3(texture(u_material.albedo,   in_texCoords));
		specular += u_pointLight[i].specular * spec * vec3(texture(u_material.specular, in_texCoords));
		diffuse  *= 1.0 / (1.0 + 0.01 * pow(length(u_pointLight[i].position - in_position), 2.0));
		specular *= 1.0 / (1.0 + 0.01 * pow(length(u_pointLight[i].position - in_position), 2.0));

	}
	
	out_color = vec4((ambient + diffuse + specular), 1.0);


	// Gamma Correction
	vec3 gamma = vec3(1.0 / 2.2);
	out_color = pow(out_color, vec4(gamma, 1.0));
}