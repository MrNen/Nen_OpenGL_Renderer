#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec3 aBiTangent;

out vec2 TexCoord;
out vec3 WorldPos;

layout (location = 0) uniform mat4 transform;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

layout (location = 0) out Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    mat3 tangentBasis;
} vout;

void main() {
    vout.position = vec3(transform * vec4(aPos, 1.0));
    vout.texcoord = aTexCoord;

    vec3 tangent = vec3(aTangent.x, aTangent.y, aTangent.z);

    vout.normal = aNormal;

    vec3 B = normalize(cross(aNormal, tangent));
    vout.tangentBasis = mat3(transform) * mat3(tangent, B, aNormal);
    gl_Position = projection * view * transform * vec4(aPos, 1.0f);
}