#version 420

#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec2 in_texCoords; 
                                           
layout(location = 0) out vec4 out_color;

layout(location = 0) uniform sampler2D u_texture;

void main()
{
    vec4 tempColor = texture(u_texture, in_texCoords);
    float average = 0.2126 * tempColor.r + 0.7152 * tempColor.g + 0.0722 * tempColor.b;
    out_color = vec4(vec3(average), 1.0);
}