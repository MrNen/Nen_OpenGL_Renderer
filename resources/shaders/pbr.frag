#version 440 core

const float PI = 3.141592;
const float Epsilon = 0.00001;

layout (location = 0) in Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    mat3 tangentBasis;
} vin;

const vec3 Fdielectric = vec3(0.04);

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D normalMap;
//Per GLTF Specs, metalness is Blue channel, roughness is Green channel.
layout (binding = 2) uniform sampler2D metallicRoughnessMap;
layout (binding = 3) uniform sampler2D ambientOcclusionMap;
layout (binding = 4) uniform sampler2D emissiveMap;

layout (location = 6) uniform vec3 ambientLight;
layout (location = 7) uniform vec3 cameraPos;
layout (location = 8) uniform vec3 lightPosition;
layout (location = 9) uniform vec3 lightColor;


layout (location = 0) out vec4 FragColor;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, vin.texcoord).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(vin.position);
    vec3 Q2 = dFdy(vin.position);
    vec2 st1 = dFdx(vin.texcoord);
    vec2 st2 = dFdy(vin.texcoord);

    vec3 N = normalize(vin.normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


void main()
{
    //converts albedo texture into linear space.
    vec3 albedo = pow(texture(albedoMap, vin.texcoord).rgb, vec3(2.2));

    float metallic = texture(metallicRoughnessMap, vin.texcoord).b;
    float roughness = texture(metallicRoughnessMap, vin.texcoord).g;
    float ao = texture(ambientOcclusionMap, vin.texcoord).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(cameraPos - vin.position);
    vec3 F0 = vec3(4.0);
    F0 = mix(F0, albedo, metallic);
    vec3 Lo = vec3(0.0);

    vec3 L = normalize(lightPosition - vin.position);
    vec3 H = normalize(V + L);

    float distance = length(lightPosition - vin.position);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}