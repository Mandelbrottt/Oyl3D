#version 420

#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec2 in_texCoords; 
                                           
layout(location = 0) out vec4 out_color;

layout(location = 0) uniform sampler2D u_screen;
layout(location = 1) uniform sampler3D u_lut;

void main()
{
    vec4 tempColor = texture(u_screen, in_texCoords);

    vec3 size = textureSize(u_lut, 0);
    vec3 tempRST = tempColor.rgb * ((size - 1.0) / size) + 0.5 * 1.0 / size;
    
    out_color = texture(u_lut, tempRST);
}