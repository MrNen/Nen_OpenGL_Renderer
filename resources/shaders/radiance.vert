#version 440 core


layout (location = 0) in vec3 pos;

out vec3 localPos;


layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;


void main() {
    localPos = pos;
    gl_Position = projection * view * vec4(localPos, 1.0);

}