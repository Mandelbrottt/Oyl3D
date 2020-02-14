#version 420

#define NUM_POINT_LIGHTS 8
#define NUM_DIR_LIGHTS   8
#define NUM_SPOT_LIGHTS  8
#define NUM_SHADOW_MAPS  2

#define POINT_SHADOW 1
#define DIR_SHADOW   2
#define SPOT_SHADOW  3

struct Material 
{
	sampler2D albedo;
	sampler2D specular;
	sampler2D normal;
	sampler2D emission;

	// float shininess;

	vec2 tiling;
	vec2 offset;
};

struct PointLight 
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float range;
};

struct DirLight 
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
	
	float range;

	float innerCutoff;
	float outerCutoff;
};

struct Shadow
{
	int type;
	sampler2D map;
};

in VS_OUT {
	vec3 position;
	vec2 texCoord;
	mat3 TBN;
	vec4 lightSpacePosition;
} fs_in;

layout(location = 0) out vec4 out_color;

uniform Material u_material;

uniform PointLight       u_pointLight[NUM_POINT_LIGHTS];
uniform DirLight 	     u_dirLight[NUM_DIR_LIGHTS];
uniform SpotLight        u_spotLight[NUM_SPOT_LIGHTS];

uniform Shadow u_shadow[NUM_SHADOW_MAPS];

vec3 calculateDirLight(DirLight light, vec3 fragPos, vec3 normal, vec2 texCoord, inout int shadowIndex);
vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec2 texCoord, inout int shadowIndex);
vec3 calculateSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec2 texCoord, inout int shadowIndex);

float shadowCalculation(vec4 lightSpacePosition, sampler2D shadowMap, vec3 normal, vec3 lightDir);

void main() 
{
	vec2 mainTexCoord = fs_in.texCoord * u_material.tiling + u_material.offset;

	// Calculate normal based on TBN and normal map
	vec3 normal;
	normal = texture(u_material.normal, mainTexCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(fs_in.TBN * normal);

	out_color.rgb = vec3(0.0, 0.0, 0.0);

	int shadowIndex = 0;

	// Calculate all point light effects
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) 
		out_color.rgb += calculatePointLight(u_pointLight[i], 
											 fs_in.position, 
											 normal, 
											 mainTexCoord, 
											 shadowIndex);

	// Calculate all directional light effects
	for (int i = 0; i < NUM_DIR_LIGHTS; i++) 
		out_color.rgb += calculateDirLight(u_dirLight[i], 
										   fs_in.position, 
										   normal, 
										   mainTexCoord, 
										   shadowIndex);

	// Calculate all spot light effects
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++)
		out_color.rgb += calculateSpotLight(u_spotLight[i], 
											fs_in.position, 
											normal, 
											mainTexCoord, 
											shadowIndex);
	
	out_color += texture(u_material.emission, mainTexCoord);

	// Gamma Correction
	vec3 gamma = vec3(1.0 / 2.2);
	out_color.rgb = pow(out_color.rgb, gamma);
	out_color.a = 1.0;
}

vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec2 texCoord, inout int shadowIndex)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 viewDir = normalize(-fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 128);

    vec3 ambient  = light.ambient  *        vec3(texture(u_material.albedo,   texCoord));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_material.albedo,   texCoord));
	vec3 specular = light.specular * spec * vec3(texture(u_material.specular, texCoord));
	
	float dist = length(light.position - fragPos);
	
	float attenuation = clamp(1.0 - (dist * dist) / (light.range * light.range), 0.0, 1.0); 
	attenuation *= attenuation;

	diffuse  *= attenuation;
	specular *= attenuation;
	
	float shadow = 0.0;
	if (shadowIndex < NUM_SHADOW_MAPS && u_shadow[shadowIndex].type == POINT_SHADOW)
		shadow = shadowCalculation(fs_in.lightSpacePosition, u_shadow[shadowIndex].map, normal, lightDir);
	return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calculateDirLight(DirLight light, vec3 fragPos, vec3 normal, vec2 texCoord, inout int shadowIndex)
{
	vec3 lightDir = normalize(-light.direction);

	// Diffuse Shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular Shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec 		= pow(max(dot(normalize(-fragPos), reflectDir), 0.0), 128);

	// Combine Results
	vec3 ambient  = light.ambient  *        vec3(texture(u_material.albedo,   texCoord));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_material.albedo,   texCoord));
	vec3 specular = light.specular * spec * vec3(texture(u_material.specular, texCoord));

	float shadow = 0.0;
	if (shadowIndex < NUM_SHADOW_MAPS && u_shadow[shadowIndex].type == DIR_SHADOW)
		shadow = shadowCalculation(fs_in.lightSpacePosition, u_shadow[shadowIndex].map, normal, lightDir);
	return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec2 texCoord, inout int shadowIndex)
{
	vec3 lightDir = normalize(light.position - fragPos);
		
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	if (theta > light.outerCutoff) 
	{
		// Diffuse Shading
		float diff = max(dot(normal, lightDir), 0.0);

		// Specular Shading
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec 		= pow(max(dot(normalize(-fragPos), reflectDir), 0.0), 128);

		// Attenuation
		float dist = length(light.position - fragPos);
	
		float attenuation = clamp(1.0 - (dist * dist) / (light.range * light.range), 0.0, 1.0); 
		attenuation *= attenuation;

		// Combine Results
		vec3 ambient  = light.ambient  *        vec3(texture(u_material.albedo,   texCoord));
		vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_material.albedo,   texCoord));
		vec3 specular = light.specular * spec * vec3(texture(u_material.specular, texCoord));

		diffuse  *= attenuation * intensity;
		specular *= attenuation * intensity;
		
		float shadow = 0.0;
		if (shadowIndex < NUM_SHADOW_MAPS && u_shadow[shadowIndex].type == SPOT_SHADOW)
			shadow = shadowCalculation(fs_in.lightSpacePosition, u_shadow[shadowIndex].map, normal, lightDir);
		return ambient + (1.0 - shadow) * (diffuse + specular);
	}
	else 
	{
		return light.ambient * texture(u_material.albedo, texCoord).rgb;
	}
}

float shadowCalculation(vec4 lightSpacePosition, sampler2D shadowMap, vec3 normal, vec3 lightDir)
{
	// Transform into NDC then into [0,1]
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords = projCoords * 0.5 + 0.5;
	
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
	
	// Check the test depth against the current depth
	float biasMin = 0.001, biasMax = 0.005;
	float bias = max(biasMax * (1.0 - dot(normal, lightDir)), biasMin); 
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
	        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	    }    
	}
	shadow /= 9.0;


	// if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
	//     projCoords.y < 0.0 || projCoords.y > 1.0)
	if (projCoords.z > 1.0)
		shadow = 0.0;

    return shadow;
}