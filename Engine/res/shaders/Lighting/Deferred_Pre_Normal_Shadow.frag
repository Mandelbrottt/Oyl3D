#version 420

struct Material 
{
	sampler2D albedo;
	sampler2D specular;
	sampler2D normal;
	sampler2D emission;

    float glossiness;

	vec2 tiling;
	vec2 offset;
};

in VS_OUT {
    vec3 position;
    vec2 texCoord;
    mat3 TBN;
    vec4 lightSpacePosition;
} fs_in;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_albedo3_specular1;
layout(location = 3) out vec3 out_normal;
layout(location = 4) out vec4 out_emission3_glossiness1;
layout(location = 5) out vec4 out_lightSpacePosition;

uniform Material u_material;


void main() {
    // Adjust the UVs to take into account offset and tiling
	vec2 adjustedTexCoords = fs_in.texCoord * u_material.tiling + u_material.offset;
    
    out_position = fs_in.position;
    
    // Store the albedo color in 
    out_albedo3_specular1.rgb = texture(u_material.albedo,   adjustedTexCoords).rgb;
    
    out_albedo3_specular1.a = texture(u_material.specular, adjustedTexCoords).r;
    
    out_emission3_glossiness1.rgb = texture(u_material.emission, adjustedTexCoords).rgb;

    out_emission3_glossiness1.a = u_material.glossiness;

    // Calculate the normal of the object based on the normal map
	out_normal = texture(u_material.normal, adjustedTexCoords).rgb; 
	out_normal = normalize(fs_in.TBN * out_normal);

    out_lightSpacePosition = fs_in.lightSpacePosition;
}