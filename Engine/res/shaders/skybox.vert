#version 420

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_texCoords;

uniform mat4 u_viewProjection;

void main()
{
    out_texCoords = in_position;
    gl_Position = u_viewProjection * vec4(in_position, 1.0);
}  