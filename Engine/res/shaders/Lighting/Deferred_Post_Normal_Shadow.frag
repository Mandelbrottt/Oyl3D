#version 420

#extension GL_ARB_explicit_uniform_location : require

#define NUM_POINT_LIGHTS 8
#define NUM_DIR_LIGHTS   8
#define NUM_SPOT_LIGHTS  8
#define NUM_SHADOW_MAPS  2

#define POINT_SHADOW 1
#define DIR_SHADOW   2
#define SPOT_SHADOW  3

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
	int       type;
	sampler2D map;
	vec2      biasMinMax;
};

layout(location = 0) in vec2 in_texCoords;

layout(location = 0) out vec3 out_color;

layout(location = 0) uniform sampler2D u_position;
layout(location = 1) uniform sampler2D u_albedo3_specular1;
layout(location = 2) uniform sampler2D u_normal;
layout(location = 3) uniform sampler2D u_emission3_glossiness1;
layout(location = 4) uniform sampler2D u_lightSpacePosition;

uniform PointLight u_pointLight[NUM_POINT_LIGHTS];
uniform DirLight   u_dirLight[NUM_DIR_LIGHTS];
uniform SpotLight  u_spotLight[NUM_SPOT_LIGHTS];

uniform Shadow u_shadow[NUM_SHADOW_MAPS];

vec3 calculateDirLight(DirLight light, inout int shadowIndex);
vec3 calculatePointLight(PointLight light, inout int shadowIndex);
vec3 calculateSpotLight(SpotLight light, inout int shadowIndex);

float shadowCalculation(vec4 lightSpacePosition, sampler2D shadowMap, vec2 biasMinMax, vec3 lightDir);

vec3 g_position;
vec4 g_albedo3_specular1;
vec3 g_normal;
vec4 g_emission3_glossiness1;
vec4 g_lightSpacePosition;

void main() 
{
	// Calculate normal based on TBN and normal map
	g_position              = texture(u_position, in_texCoords).rgb;
	g_albedo3_specular1     = texture(u_albedo3_specular1, in_texCoords);
	g_normal                = texture(u_normal, in_texCoords).rgb;
	g_emission3_glossiness1 = texture(u_emission3_glossiness1, in_texCoords);
    g_lightSpacePosition    = texture(u_lightSpacePosition, in_texCoords);

	out_color.rgb = vec3(0.0, 0.0, 0.0);

	int shadowIndex = 0;

	// Calculate all point light effects
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) 
		out_color.rgb += calculatePointLight(u_pointLight[i], shadowIndex);

	// Calculate all directional light effects
	for (int i = 0; i < NUM_DIR_LIGHTS; i++) 
		out_color.rgb += calculateDirLight(u_dirLight[i], shadowIndex);

	// Calculate all spot light effects
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++)
		out_color.rgb += calculateSpotLight(u_spotLight[i], shadowIndex);
	
	out_color.rgb += texture(u_emission3_glossiness1, in_texCoords).rgb;

	// Gamma Correction
	vec3 gamma = vec3(1.0 / 2.2);
	out_color.rgb = pow(out_color.rgb, gamma);
}

vec3 calculatePointLight(PointLight light, inout int shadowIndex)
{
	vec3 lightDir = normalize(light.position - g_position);

	float diff = max(dot(g_normal, lightDir), 0.0);

	vec3 viewDir = normalize(-g_position);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	
	float spec = pow(max(dot(g_normal, halfwayDir), 0.0), 128);

    vec3 ambient  = light.ambient  *        g_albedo3_specular1.rgb;
	vec3 diffuse  = light.diffuse  * diff * g_albedo3_specular1.rgb;
	vec3 specular = light.specular * spec * g_albedo3_specular1.a;
	
	float dist = length(light.position - g_position);
	
	float attenuation = clamp(1.0 - (dist * dist) / (light.range * light.range), 0.0, 1.0); 
	attenuation *= attenuation;

	diffuse  *= attenuation;
	specular *= attenuation;
	
	float shadow = 0.0;
	if (shadowIndex < NUM_SHADOW_MAPS && u_shadow[shadowIndex].type == POINT_SHADOW)
		shadow = shadowCalculation(g_lightSpacePosition, u_shadow[shadowIndex].map, u_shadow[shadowIndex].biasMinMax, lightDir);
	return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calculateDirLight(DirLight light, inout int shadowIndex)
{
	vec3 lightDir = normalize(-light.direction);

	// Diffuse Shading
	float diff = max(dot(g_normal, lightDir), 0.0);

	// Specular Shading
	vec3 reflectDir = reflect(-lightDir, g_normal);
	float spec 		= pow(max(dot(normalize(-g_position), reflectDir), 0.0), 128);

	// Combine Results
    vec3 ambient  = light.ambient  *        g_albedo3_specular1.rgb;
	vec3 diffuse  = light.diffuse  * diff * g_albedo3_specular1.rgb;
	vec3 specular = light.specular * spec * g_albedo3_specular1.a;

	float shadow = 0.0;
	if (shadowIndex < NUM_SHADOW_MAPS && u_shadow[shadowIndex].type == DIR_SHADOW)
		shadow = shadowCalculation(g_lightSpacePosition, u_shadow[shadowIndex].map, u_shadow[shadowIndex].biasMinMax, lightDir);
	return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, inout int shadowIndex)
{
	vec3 lightDir = normalize(light.position - g_position);
		
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	if (theta > light.outerCutoff) 
	{
		// Diffuse Shading
		float diff = max(dot(g_normal, lightDir), 0.0);

		// Specular Shading
		vec3 reflectDir = reflect(-lightDir, g_normal);
		float spec 		= pow(max(dot(normalize(-g_position), reflectDir), 0.0), 128);

		// Attenuation
		float dist = length(light.position - g_position);
	
		float attenuation = clamp(1.0 - (dist * dist) / (light.range * light.range), 0.0, 1.0); 
		attenuation *= attenuation;

		// Combine Results
        vec3 ambient  = light.ambient  *        g_albedo3_specular1.rgb;
	    vec3 diffuse  = light.diffuse  * diff * g_albedo3_specular1.rgb;
	    vec3 specular = light.specular * spec * g_albedo3_specular1.a;

		diffuse  *= attenuation * intensity;
		specular *= attenuation * intensity;
		
		float shadow = 0.0;
		if (shadowIndex < NUM_SHADOW_MAPS && u_shadow[shadowIndex].type == SPOT_SHADOW)
			shadow = shadowCalculation(g_lightSpacePosition, u_shadow[shadowIndex].map, u_shadow[shadowIndex].biasMinMax, lightDir);
		return ambient + (1.0 - shadow) * (diffuse + specular);
	}
	else 
	{
		return light.ambient * g_albedo3_specular1.rgb;
;
	}
}

float shadowCalculation(vec4 lightSpacePosition, sampler2D shadowMap, vec2 biasMinMax, vec3 lightDir)
{
	// Transform into NDC then into [0,1]
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords = projCoords * 0.5 + 0.5;
	
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
	
	// Check the test depth against the current depth
	// float biasMin = 0.001, biasMax = 0.005;
	float bias = max(biasMinMax.y * (1.0 - dot(g_normal, lightDir)), biasMinMax.x); 
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

	// TODO: Do Variance Shadow Mapping

	// if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
	//     projCoords.y < 0.0 || projCoords.y > 1.0)
	if (projCoords.z > 1.0)
		shadow = 0.0;

    return shadow;
}