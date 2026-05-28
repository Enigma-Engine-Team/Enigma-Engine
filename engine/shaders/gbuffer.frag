#version 460 core
layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 WorldPos;
layout(location = 2) in vec3 Normal;

layout(binding = 3)  uniform sampler2D albedoMap;
layout(binding = 4)  uniform sampler2D normalMap;
layout(binding = 5)  uniform sampler2D metallicRoughnessMap;
layout(binding = 6)  uniform sampler2D roughnessMap;
layout(binding = 7)  uniform sampler2D aoMap;
layout(binding = 8)  uniform sampler2D clearCoatMap;
layout(binding = 9)  uniform sampler2D clearCoatRoughMap;
layout(binding = 10) uniform sampler2D clearCoatNormalMap;
layout(binding = 16) uniform sampler2D emissiveMap;

struct Material
{
    vec4  albedoColor;
    vec4  emissiveColorIntensity; // color -> XYZ, intensity = W
    float metallic;
    float roughness;
    float ao;
    float normalStrength;
    float clearCoatFactor;
    float clearCoatRoughFactor;
    float padding1;
    float padding2;
    int   useAlbedoTexture;
    int   useNormalTexture;
    int   useMetallicRoughnessTexture;
    int   useMetallicTexture;
    int   useRoughnessTexture;
    int   useAOTexture;
    int   useClearCoatTexture;
    int   useClearCoatRoughTexture;
    int   useClearCoatNormalTexture;
    int   useEmissiveTexture;
    float padding3;
    float padding4;
};

layout(binding = 11) uniform MaterialData
{
    Material material;
} materialData;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoAO;
layout(location = 3) out vec2 gMetalRough;
layout(location = 4) out vec2 gClearCoatFactors; // factor (R) + roughness (G)
layout(location = 5) out vec3 gClearCoatNormal;
layout(location = 6) out vec3 gEmissive;

vec3 ComputeNormalFromMap(sampler2D map, float strength)
{
    vec3 tangentNormal = texture(map, TexCoords).xyz * 2.0 - 1.0;
    tangentNormal.xy *= strength;
    tangentNormal = normalize(tangentNormal);
    
    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);
    
    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    
    T = normalize(T - dot(T, N) * N);
    
    vec3 B = -normalize(cross(T, N));
    
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

void main()
{
    Material mat = materialData.material;

    // Albedo
    vec3 albedo = mat.useAlbedoTexture == 1
        ? texture(albedoMap, TexCoords).rgb
        : mat.albedoColor.rgb;

    // Metallic / Roughness
    float metallic  = mat.metallic;
    float roughness = mat.roughness;
    if (mat.useMetallicRoughnessTexture == 1)
    {
        metallic  = texture(metallicRoughnessMap, TexCoords).b;
        roughness = texture(metallicRoughnessMap, TexCoords).g;
    }
    else
    {
        if (mat.useMetallicTexture  == 1) metallic  = texture(metallicRoughnessMap, TexCoords).r;
        if (mat.useRoughnessTexture == 1) roughness = texture(roughnessMap, TexCoords).r;
    }

    // AO
    float ao = mat.useAOTexture == 1 ? texture(aoMap, TexCoords).r : mat.ao;

    // Normal
    vec3 normal = mat.useNormalTexture == 1
        ? ComputeNormalFromMap(normalMap, mat.normalStrength)
        : normalize(Normal);

    float ccFactor = mat.clearCoatFactor;
    float ccRough  = mat.clearCoatRoughFactor;
    if (mat.useClearCoatTexture      == 1) ccFactor  *= texture(clearCoatMap,      TexCoords).r;
    if (mat.useClearCoatRoughTexture == 1) ccRough  *= texture(clearCoatRoughMap, TexCoords).g;

    // Clear coat normal
    vec3 ccNormal = normalize(normal);
    if (mat.useClearCoatNormalTexture == 1)
        ccNormal = ComputeNormalFromMap(clearCoatNormalMap, 1.0);

    // Emissive

    vec3 emissive = mat.emissiveColorIntensity.xyz;

    if(mat.useEmissiveTexture == 1)
        emissive *= texture(emissiveMap, TexCoords).rgb;

    emissive *= mat.emissiveColorIntensity.w;

    // Outputs
    gPosition          = WorldPos;
    gNormal            = normal;
    gAlbedoAO          = vec4(albedo, ao);
    gMetalRough        = vec2(metallic, roughness);
    gClearCoatFactors  = vec2(ccFactor, ccRough);
    gClearCoatNormal   = ccNormal * 0.5 + 0.5;
    gEmissive          = emissive;
}