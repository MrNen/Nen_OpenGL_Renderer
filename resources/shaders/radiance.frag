#version 440 core

out vec4 FragColor;

in vec3 localPos;


uniform sampler2D equiractangularMap;

const vec2 inverseArcTan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 vec) {
    vec2 uv = vec2(atan(vec.z, vec.x), asin(vec.y));
    uv *= inverseArcTan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equiractangularMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}