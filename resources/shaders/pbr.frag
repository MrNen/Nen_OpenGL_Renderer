#version 440 core

const float PI = 3.141592;
const float Epsilon = 0.00001;

layout (location = 0) in Vertex
{
    vec3 position;
    vec2 texcoord;
    mat3 tangentBasis;
} vin;

const vec3 Fdielectric = vec3(0.04);

uniform sampler2D albedoMap;
uniform sampler2D normalMap;

//Per GLTF Specs, metalness is Blue channel, roughness is Green channel.
uniform sampler2D metallicRoughnessMap;
uniform sampler2D ambientOcclusionMap;
uniform sampler2D emissiveMap;

layout (location = 6) uniform vec3 ambientLight;
layout (location = 7) uniform vec3 cameraPos;
layout (location = 8) uniform vec3 lightPosition;
layout (location = 9) uniform vec3 lightColor;


layout (location = 0) out vec4 color;

float ndfGGX(float cosLh, float roughness)
{
    float alpha4 = roughness * roughness * roughness * roughness;

    float denom = (cosLh * cosLh) * (alpha4 - 1.0) + 1.0;
    return alpha4 / (PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec3 albedo = texture(albedoMap, vin.texcoord).rgb;
    float metallic = texture(metallicRoughnessMap, vin.texcoord).b;
    float roughness = texture(metallicRoughnessMap, vin.texcoord).g;

    vec3 Lo = normalize(cameraPos - vin.position);
    vec3 N = normalize(2.0 * texture(normalMap, vin.texcoord).rgb - 1.0);
    N = normalize(vin.tangentBasis * N);

    float cosLo = max(0.0, dot(N, Lo));
    vec3 Lr = 2.0 * cosLo * N - Lo;
    vec3 F0 = mix(Fdielectric, albedo, metallic);

    vec3 directLighting = vec3(0);
    {
        vec3 Li = normalize(vin.position - lightPosition  );
        vec3 Lradiance = lightColor;

        vec3 Lh = normalize(Li + Lo);

        float cosLi = max(0.0, dot(N, Li));
        float cosLh = max(0.0, dot(N, Lh));

        vec3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
        float D = ndfGGX(cosLh, roughness);
        float G = gaSchlickGGX(cosLi, cosLo, roughness);

        vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

        vec3 diffuseBRDF = kd * albedo;
        vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

        directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }


    color = vec4(directLighting , 1.0);
}