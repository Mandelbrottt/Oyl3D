#version 420 core

layout(location = 0) out vec4 fragColor;

in vec2 v_texCoord;
in vec3 v_normal;

uniform sampler2D u_texture;

void main() {
	fragColor = texture(u_texture, v_texCoord);
}