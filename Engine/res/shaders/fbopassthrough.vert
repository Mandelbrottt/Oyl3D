#version 420

//#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoords;
                                           
layout(location = 0) out vec2 out_texCoords;

uniform mat4 u_model;

void main()
{
    gl_Position = u_model * vec4(in_position, 1.0);

    out_texCoords = in_texCoords;
}