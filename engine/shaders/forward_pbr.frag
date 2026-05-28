#version 460 core

const int TRUE = 1;
const int FALSE = 0;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 WorldPos;
layout(location = 2) in vec3 Normal;

out vec4 FragColor;

layout(binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
    mat4 vp;
    vec3 viewPos;
    int  padding;
} cam;

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
};

// Lights
struct LightData
{
    vec3  position;   int   type;
    vec3  color;      float intensity;
    vec3  direction;  float range;
    float innerCutOff;
    float outerCutOff;
    int   padding0;
    int   padding1;
};

layout(std430, binding = 12) buffer LightBlock
{
    int lightCount;
    int padding[3];
    LightData lights[];
} lightBuffer;

layout(binding = 13) uniform samplerCube irradianceMap;
layout(binding = 14) uniform samplerCube prefilterMap;
layout(binding = 15) uniform sampler2D   brdfLUT;

const float PI = 3.14159265359;

vec3 safeNormalize(vec3 v, vec3 fallback)
{
    float len = length(v);

    if(len > 0.0001)
    return v / len;
    else
    return fallback;
}

void DirectionalLight(out vec3 L, out vec3 radiance, int i)
{
    L = safeNormalize(-lightBuffer.lights[i].direction, vec3(0.0, 1.0, 0.0));
    radiance = lightBuffer.lights[i].color * lightBuffer.lights[i].intensity;
}

void PointLight(out vec3 L, out vec3 radiance, int i, vec3 worldPos)
{
    vec3  dir         = lightBuffer.lights[i].position - worldPos;
    float dist        = length(dir);
    L                 = safeNormalize(dir, vec3(0.0, 1.0, 0.0));
    float safeRange = max(lightBuffer.lights[i].range, 0.0001);
    float attenuation = pow(clamp(1.0 - dist / safeRange, 0.0, 1.0), 2.0);
    radiance          = lightBuffer.lights[i].color * lightBuffer.lights[i].intensity * attenuation;
}

void SpotLight(out vec3 L, out vec3 radiance, int i, vec3 worldPos)
{
    vec3  dir         = lightBuffer.lights[i].position - worldPos;
    float dist        = length(dir);
    L                 = safeNormalize(dir, vec3(0.0, 1.0, 0.0));
    float safeRange   = max(lightBuffer.lights[i].range, 0.0001);
    float attenuation = pow(clamp(1.0 - dist / safeRange, 0.0, 1.0), 2.0);
    vec3  spotDir     = safeNormalize(-lightBuffer.lights[i].direction, vec3(0.0, -1.0, 0.0));
    float theta       = dot(L, spotDir);
    float cosInner    = cos(radians(lightBuffer.lights[i].innerCutOff * 0.5));
    float cosOuter    = cos(radians(lightBuffer.lights[i].outerCutOff * 0.5));
    float epsilon     = max(cosInner - cosOuter, 0.0001);
    float spotI       = clamp((theta - cosOuter) / epsilon, 0.0, 1.0);
    radiance          = lightBuffer.lights[i].color * lightBuffer.lights[i].intensity * attenuation * spotI;
}

// PBR functions

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float alpha        = roughness * roughness;
    float alphaSquared = alpha * alpha;
    float NdotH        = max(dot(N, H), 0.0001);
    float NdotH2       = NdotH * NdotH;

    float denom = (NdotH2 * (alphaSquared - 1.0) + 1.0);
    return alphaSquared / max(PI * denom * denom, 0.0001);
}

float GeometrySchlickGGX(vec3 n, vec3 v, float k)
{
    float nDotV = max(dot(n, v), 0.0001);
    return nDotV / (nDotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return GeometrySchlickGGX(N, V, k) * GeometrySchlickGGX(N, L, k);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0001), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 ComputeNormalFromMap(sampler2D map, float strength, bool isFrontFace)
{
    vec3 tangentNormal = texture(map, TexCoords).xyz * 2.0 - 1.0;
    tangentNormal.xy *= strength;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N = normalize(Normal);
    vec3 T = safeNormalize(Q1 * st2.t - Q2 * st1.t, cross(N, vec3(0.0, 1.0, 0.0)));
    vec3 B = safeNormalize(cross(N, T), cross(N, vec3(1.0, 0.0, 0.0)));

    float faceScalar = isFrontFace ? 1.0 : -1.0;
    mat3 TBN = mat3(T * faceScalar, B * faceScalar, N * faceScalar);

    return safeNormalize(TBN * tangentNormal, N * faceScalar);
}

void main()
{
    vec3 albedo;
    if (material.useAlbedoTexture == TRUE)
    {
        albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    }
    else
    {
        albedo = material.albedoColor.rgb;
    }

    float metallic  = material.metallic;
    float roughness = material.roughness;

    if (material.useMetallicRoughnessTexture == TRUE)
    {
        vec4 mr = texture(metallicRoughnessMap, TexCoords);
        metallic  *= mr.b;
        roughness *= mr.g;
    }
    else
    {
        if (material.useMetallicTexture  == TRUE) metallic  *= texture(metallicRoughnessMap, TexCoords).r;
        if (material.useRoughnessTexture == TRUE) roughness *= texture(roughnessMap, TexCoords).r;
    }

    // AO
    float ao = material.useAOTexture == TRUE ? texture(aoMap, TexCoords).r : material.ao;

    bool isFront = gl_FrontFacing;

    // Normal mapping
    vec3 N;
    if (material.useNormalTexture == TRUE)
    {
        N = ComputeNormalFromMap(normalMap, material.normalStrength, isFront);
    }
    else
    {
        N = normalize(Normal);
        if (!isFront)
        N = -N;
    }

    // Clear coat parameters
    float ccFactor = material.clearCoatFactor;
    float ccRough  = material.clearCoatRoughFactor;
    if (material.useClearCoatTexture      == TRUE) ccFactor *= texture(clearCoatMap,      TexCoords).r;
    if (material.useClearCoatRoughTexture == TRUE) ccRough  *= texture(clearCoatRoughMap, TexCoords).g;

    // Clear coat normal
    vec3 ccNormal = N;
    if (material.useClearCoatNormalTexture == TRUE)
    {
        ccNormal = ComputeNormalFromMap(clearCoatNormalMap, 1.0, isFront);
    }
    else if (!isFront)
    {
        ccNormal = -ccNormal;
    }

    // Emissive

    vec3 emissive = material.emissiveColorIntensity.xyz;

    if(material.useEmissiveTexture == 1)
        emissive *= texture(emissiveMap, TexCoords).rgb;

    emissive *= material.emissiveColorIntensity.w;


    vec3 V = safeNormalize(cam.viewPos - WorldPos, vec3(0.0, 0.0, 1.0));
    vec3 R = reflect(-V, N);
    R = normalize(mix(R, N, 1.0 - smoothstep(0.0, 0.1, dot(N, V))));

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 LoDiffuse = vec3(0.0);
    vec3 LoSpecular = vec3(0.0);
    vec3 LoClearCoat = vec3(0.0);
    vec3 F0_cc = vec3(0.04);

    // Direct lighting
    for (int i = 0; i < lightBuffer.lightCount; ++i)
    {
        vec3 L;
        vec3 radiance = vec3(0.0);
        int type = int(lightBuffer.lights[i].type);

        if (type == 0) DirectionalLight(L, radiance, i);
        else if (type == 1) SpotLight(L, radiance, i, WorldPos);
        else if (type == 2) PointLight(L, radiance, i, WorldPos);

        vec3 H = normalize(V + L);  // Calcul� une seule fois

        // Base layer PBR
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0001), F0);

        vec3 spec = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0001) * max(dot(N, L), 0.0001), 0.0001);
        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0001);

        LoDiffuse  += (kD * albedo / PI) * radiance * NdotL;
        LoSpecular += spec * radiance * NdotL;

        // Clear coat layer
        float NDF_cc = DistributionGGX(ccNormal, H, ccRough);
        float G_cc = GeometrySmith(ccNormal, V, L, ccRough);
        vec3 F_cc = fresnelSchlick(max(dot(H, V), 0.0001), F0_cc);

        vec3 spec_cc = (NDF_cc * G_cc * F_cc) / max(4.0 * max(dot(ccNormal, V), 0.0001) * max(dot(ccNormal, L), 0.0001), 0.0001);
        float NdotL_cc = max(dot(ccNormal, L), 0.0001);
        LoClearCoat += spec_cc * radiance * NdotL_cc;
    }

    // IBL
    vec3 F_ibl = fresnelSchlickRoughness(max(dot(N, V), 0.0001), F0, roughness);
    vec3 kD_ibl = (1.0 - F_ibl) * (1.0 - metallic);

    vec3 irradiance = textureLod(irradianceMap, N, 4.0).rgb;
    vec3 diffuse = irradiance * albedo;
    float mipLevel = clamp(roughness, 0.04, 1.0) * 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, mipLevel).rgb;
    prefilteredColor = min(prefilteredColor, vec3(50.0));
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.001), clamp(roughness, 0.04, 0.99))).rg;
    vec3 specular_ibl = prefilteredColor * (F_ibl * brdf.x + brdf.y);

    vec3 ambientDiffuse = kD_ibl * diffuse * ao;
    vec3 ambientSpecular = specular_ibl * ao;

    // Clear coat IBL
    vec3 R_cc = reflect(-V, ccNormal);
    vec3 F_cc_ibl = fresnelSchlickRoughness(max(dot(ccNormal, V), 0.0001), F0_cc, ccRough);
    vec3 prefilter_cc = textureLod(prefilterMap, R_cc, ccRough * 4.0).rgb;
    vec2 brdf_cc = texture(brdfLUT, vec2(max(dot(ccNormal, V), 0.0001), ccRough)).rg;
    vec3 specular_cc_ibl = prefilter_cc * (F_cc_ibl * brdf_cc.x + brdf_cc.y);

    // Energy conservation avec le clear coat
    float ccReflectivity = (F_cc_ibl.r + F_cc_ibl.g + F_cc_ibl.b) / 3.0;
    float ccEnergy = 1.0 - ccFactor * ccReflectivity;

    vec3 totalDiffuse = (ambientDiffuse + LoDiffuse) * ccEnergy;
    vec3 totalSpecular = (ambientSpecular + LoSpecular) * ccEnergy + (LoClearCoat + specular_cc_ibl) * ccFactor;

    // Alpha calculation
    float textureAlpha = material.useAlbedoTexture == TRUE ? texture(albedoMap, TexCoords).a : 1.0;
    float baseAlpha = textureAlpha * material.albedoColor.a;
    float fresnelAlpha = baseAlpha + (1.0 - baseAlpha) * pow(1.0 - max(dot(N, V), 0.0001), 5.0);

    vec3 color = totalDiffuse + totalSpecular + emissive;

    FragColor = vec4(color, fresnelAlpha);
}