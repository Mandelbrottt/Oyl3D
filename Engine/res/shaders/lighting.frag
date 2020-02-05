#version 420

#define NUM_LIGHTS 8

struct Material 
{
	sampler2D albedo;
	sampler2D specular;
	sampler2D normal;
	sampler2D emission;

	vec2 tiling;
	vec2 offset;
};

struct PointLight 
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float attenK;
	float attenL;
	float attenQ;
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

in VS_OUT {
	vec3 position;
	vec2 texCoord;
	mat3 TBN;
} fs_in;

layout(location = 0) out vec4 out_color;

uniform Material u_material;

uniform PointLight u_pointLight[NUM_LIGHTS];

void main() 
{
	vec2 mainTexCoord = fs_in.texCoord * u_material.tiling + u_material.offset;

	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	for (int i = 0; i < NUM_LIGHTS; i++) 
	{
		vec3 normal;
		normal = texture(u_material.normal, mainTexCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(fs_in.TBN * normal);

		vec3 lightDir = normalize(u_pointLight[i].position - fs_in.position);

		float diff = max(dot(normal, lightDir), 0.0);

		vec3 viewDir = normalize(-fs_in.position);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		float spec = pow(max(dot(normal, halfwayDir), 0.0), 128);

    	vec3 tempambient  = u_pointLight[i].ambient  *        vec3(texture(u_material.albedo,   mainTexCoord));
		vec3 tempdiffuse  = u_pointLight[i].diffuse  * diff * vec3(texture(u_material.albedo,   mainTexCoord));
		vec3 tempspecular = u_pointLight[i].specular * spec * vec3(texture(u_material.specular, mainTexCoord));

		float dist = length(u_pointLight[i].position - fs_in.position);
		float Kc = u_pointLight[i].attenK;
		float Kl = u_pointLight[i].attenL * dist;
		float Kq = u_pointLight[i].attenQ * dist * dist;
		float attenuation = 0.0;
		if (Kc + Kl + Kq != 0)
			attenuation = 1.0 / (Kc + Kl + Kq);
		// float attenuation = 1.0 / (1.0 + 0.1 * dist * dist);

		
		tempdiffuse  *= attenuation;
		tempspecular *= attenuation;
		// tempdiffuse = vec3(Kc, Kl, Kq);
		
		ambient += tempambient;
		diffuse += tempdiffuse;
		specular += tempspecular;
	}
	
	out_color = vec4((ambient + diffuse + specular), 1.0);
	out_color += texture(u_material.emission, mainTexCoord);

	// Gamma Correction
	vec3 gamma = vec3(1.0 / 2.2);
	out_color = pow(out_color, vec4(gamma, 1.0));
}