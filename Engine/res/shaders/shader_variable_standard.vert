#version 420

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoords;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_texCoords;
layout(location = 2) out vec3 out_normal;

uniform mat4 u_modelView;
uniform mat4 u_modelViewProjection;
uniform mat3 u_modelViewNormal;

void main()
{
    gl_Position = u_modelViewProjection * vec4(in_position, 1.0);

    out_position = vec3(u_modelView * vec4(in_position, 1.0));
    out_normal = u_modelViewNormal * in_normal;
}
