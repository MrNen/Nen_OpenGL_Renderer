#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec3 aBiTangent;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;

layout(location = 0) uniform mat4 transform;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;



void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0f);
    WorldPos = vec3(transform * vec4(aPos, 1.0));
    Normal = (transpose(inverse(mat3(transform)))) * aNormal;
    TexCoord = aTexCoord;
}