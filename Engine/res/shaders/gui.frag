#version 420

layout(location = 0) in vec2 in_texCoords; 
                                           
layout(location = 0) out vec4 out_color;

uniform sampler2D u_texture;

uniform vec4 u_clippingCoords;

void main()
{
    if (in_texCoords.x < u_clippingCoords.x ||
        in_texCoords.y < u_clippingCoords.y ||
        in_texCoords.x > u_clippingCoords.z ||
        in_texCoords.y > u_clippingCoords.w)
    discard;

    out_color = texture(u_texture, in_texCoords);
}