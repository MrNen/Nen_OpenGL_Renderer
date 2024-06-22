#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}