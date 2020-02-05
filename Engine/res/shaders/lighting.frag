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

uniform sampler1D u_ramp;

uniform bool u_doLighting     = true;
uniform bool u_doAmbient      = true;
uniform bool u_doSpecular     = true;
uniform bool u_doRim          = true;
uniform bool u_doDiffuseRamp  = false;
uniform bool u_doSpecularRamp = false;

void main() 
{
	vec2 mainTexCoord = fs_in.texCoord * u_material.tiling + u_material.offset;

	vec3 ambient  = vec3(0.0);
	vec3 diffuse  = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 rim      = vec3(0.0);

	for (int i = 0; i < NUM_LIGHTS; i++) 
	{
		vec3 normal;
		normal = texture(u_material.normal, mainTexCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(fs_in.TBN * normal);

		vec3 lightDir = normalize(u_pointLight[i].position - fs_in.position);

		float diff = max(dot(normal, lightDir), 0.0);

		if (u_doDiffuseRamp)
			diff = texture(u_ramp, diff).r;

		vec3 viewDir = normalize(-fs_in.position);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		float spec = max(dot(normal, halfwayDir), 0.0);
		spec = pow(spec, 128);
		
		if (u_doSpecularRamp)
			spec = texture(u_ramp, spec).r;

    	vec3 tempambient  = u_pointLight[i].ambient  *        vec3(texture(u_material.albedo,   mainTexCoord));
		vec3 tempdiffuse  = u_pointLight[i].diffuse  * diff * vec3(texture(u_material.albedo,   mainTexCoord));
		vec3 tempspecular = u_pointLight[i].specular * spec * vec3(texture(u_material.specular, mainTexCoord));

		vec3 rimColor = (vec3(u_pointLight[i].diffuse) ) / 1;
		float rimVal = 1 - max(dot(normal, -normalize(fs_in.position)), 0.0);
		rimVal = smoothstep(0.4, 1.0, rimVal);
		vec3 tempRim = diff * rimColor * vec3(rimVal, rimVal, rimVal);

		float dist = length(u_pointLight[i].position - fs_in.position);
		float attenuation = 1.0 / (1.0 + 0.01 * dist * dist);
		
		tempdiffuse  *= attenuation;
		tempspecular *= attenuation;
		tempRim *= attenuation;

		if (u_doAmbient)
			ambient += tempambient;

		diffuse += tempdiffuse;

		if (u_doSpecular)
			specular += tempspecular;
		
		if (u_doRim)
			rim += tempRim;
	}
	
	if (!u_doLighting) {
		rim      = vec3(0);
		ambient  = vec3(0);
		specular = vec3(0);
		diffuse = texture(u_material.albedo, mainTexCoord).rgb;
	}

	out_color = vec4((rim + ambient + diffuse + specular), 1.0);
	out_color += texture(u_material.emission, mainTexCoord);

	// Gamma Correction
	vec3 gamma = vec3(1.0 / 2.2);
	out_color = pow(out_color, vec4(gamma, 1.0));
}