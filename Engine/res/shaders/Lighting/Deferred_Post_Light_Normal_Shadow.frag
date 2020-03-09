#version 420

#extension GL_ARB_explicit_uniform_location : require

struct PointLight 
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float intensity;
	float range;
};

layout(location = 0) in vec2 in_texCoords;

layout(location = 0) out vec3 out_color;

layout(binding = 0) uniform sampler2D u_position;
layout(binding = 1) uniform sampler2D u_albedo3_specular1;
layout(binding = 2) uniform sampler2D u_normal;
layout(binding = 3) uniform sampler2D u_emission3_glossiness1;
layout(binding = 4) uniform sampler2D u_lightSpacePosition;

uniform PointLight u_pointLight;

vec3 calculatePointLight(PointLight light, inout int shadowIndex);

vec3 g_position;
vec4 g_albedo3_specular1;
vec3 g_normal;
vec4 g_emission3_glossiness1;
vec4 g_lightSpacePosition;

void main() 
{
	// Grab the values at the current pixel
	g_position              = texture(u_position, in_texCoords).rgb;
	g_albedo3_specular1     = texture(u_albedo3_specular1, in_texCoords);
	g_normal                = texture(u_normal, in_texCoords).rgb;
	g_emission3_glossiness1 = texture(u_emission3_glossiness1, in_texCoords);
	g_lightSpacePosition = texture(u_lightSpacePosition, in_texCoords);

	out_color.rgb = vec3(0.0, 0.0, 0.0);

	int shadowIndex = 0;

	// Calculate all point light effects
	out_color.rgb += calculatePointLight(u_pointLight, shadowIndex);
	
	// Add emission
	out_color.rgb += texture(u_emission3_glossiness1, in_texCoords).rgb;
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

	diffuse  *= attenuation * light.intensity;
	specular *= attenuation * light.intensity;
	
	return ambient + (diffuse + specular);
}