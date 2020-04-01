#version 420

#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec2 in_texCoords; 
                                           
layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D u_hdrBuffer;

layout(location = 0) uniform float u_exposure   = 1.0;

layout(location = 1) uniform mat4 u_brightness = mat4(1.0);
layout(location = 2) uniform mat4 u_contrast   = mat4(1.0);
layout(location = 3) uniform mat4 u_saturation = mat4(1.0);

const float g_gamma = 2.2;
const float g_oneOverGamma = 1.0 / g_gamma;

void main()
{
    vec3 hdrColor = texture(u_hdrBuffer, in_texCoords).rgb;
  
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * u_exposure);

    // Gamma correction 
    mapped = pow(mapped, vec3(g_oneOverGamma));
  
    // out_color = vec4(mapped, 1.0);

    out_color = u_brightness * 
                u_contrast * 
                u_saturation * 
                vec4(mapped, 1.0);
}