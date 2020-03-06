#version 420

layout(location = 0) in vec3  in_position;
layout(location = 1) in vec2  in_texCoord;
layout(location = 2) in vec3  in_normal;
layout(location = 3) in vec3  in_tangent;
layout(location = 4) in vec3  in_bitangent;
layout(location = 5) in ivec4 in_bones;
layout(location = 6) in vec4  in_boneWeights;

out VS_OUT {
    vec3 position;
    vec2 texCoord;
    mat3 TBN;
    vec4 lightSpacePosition;
} vs_out;

uniform mat4 u_model;

uniform mat4 u_view;
uniform mat4 u_viewProjection;
uniform mat3 u_viewNormal;
uniform mat4 u_lightSpaceMatrix;

uniform mat4 u_boneTransforms[64] = { mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), 
                                      mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), 
                                      mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), 
                                      mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), 
                                      mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), 
                                      mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), 
                                      mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), 
                                      mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), mat4(1.0), };

void main() 
{
    mat4 boneTransform = mat4(0.0);
    boneTransform += u_boneTransforms[in_bones[0]] * in_boneWeights[0];
    boneTransform += u_boneTransforms[in_bones[1]] * in_boneWeights[1];
    boneTransform += u_boneTransforms[in_bones[2]] * in_boneWeights[2];
    boneTransform += u_boneTransforms[in_bones[3]] * in_boneWeights[3];
    // boneTransform = mat4(1.0);

    mat4 boneModel = u_model * boneTransform;
    // mat4 boneModel = boneTransform;

    vec4 modelPosition = boneModel * vec4(in_position, 1.0);

    gl_Position = u_viewProjection * modelPosition;

    vs_out.position = vec3(u_view * modelPosition);
    vs_out.texCoord = in_texCoord;

    vs_out.lightSpacePosition = u_lightSpaceMatrix * modelPosition;

    mat3 viewNormalModel = u_viewNormal * mat3(boneModel);

    vec3 T = normalize(viewNormalModel * in_tangent);
    vec3 B = normalize(viewNormalModel * in_bitangent);
    vec3 N = normalize(viewNormalModel * in_normal);

    vs_out.TBN = mat3(T, B, N);

    // out_normal = u_viewNormal * mat3(u_model) * in_normal;
}