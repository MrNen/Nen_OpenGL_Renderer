#version 440 core

layout (location = 0) in vec3 pos;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    TexCoords = pos;
    gl_Position = projection * view * vec4(pos, 1.0);
}