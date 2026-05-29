#version 460 core

#define MAX_SPOT_LIGHTS      10
#define MAX_POINT_LIGHTS     6
#define SPOT_SHADOW_BINDING  16
#define POINT_SHADOW_BINDING (SPOT_SHADOW_BINDING + MAX_SPOT_LIGHTS)

in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// ── G-Buffer ──────────────────────────────────────────────────────────────────
layout(binding = 0)  uniform sampler2D gPosition;
layout(binding = 1)  uniform sampler2D gNormal;
layout(binding = 2)  uniform sampler2D gAlbedoAO;
layout(binding = 3)  uniform sampler2D gMetalRough;
layout(binding = 10) uniform sampler2D gClearCoatFactors;
layout(binding = 11) uniform sampler2D gClearCoatNormal;
layout(binding = 14) uniform sampler2D gEmissive;
layout(binding = 15) uniform sampler2D gSSAO;

// ── Camera ────────────────────────────────────────────────────────────────────
layout(binding = 4) uniform CameraData
{
    mat4 projection;
    mat4 view;
    mat4 vp;
    vec3 viewPos;
    int  padding;
} cam;

// ── Lumières ──────────────────────────────────────────────────────────────────
struct LightData
{
    vec3 position;
	int lightType;

	vec3 color;
	float intensity;

	vec3 direction;
	float range;

	float innerCutOff;
	float outerCutOff;

	int padding0;
	int padding1;

	mat4 lightSpaceMatrix;
};

layout(std430, binding = 5) buffer LightBlock
{
    int lightCount;
    int padding[3];
    LightData lights[];
} lightBuffer;

// ── IBL ───────────────────────────────────────────────────────────────────────
layout(binding = 6) uniform samplerCube irradianceMap;
layout(binding = 7) uniform samplerCube prefilterMap;
layout(binding = 8) uniform sampler2D   brdfLUT;

layout (std140, binding = 13) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];
uniform int cascadeCount;
layout(binding = 12) uniform sampler2DArray shadowMap;
uniform float shadowFarPlane;

layout(binding = SPOT_SHADOW_BINDING)  uniform sampler2D   spotShadowMaps[MAX_SPOT_LIGHTS];
layout(binding = POINT_SHADOW_BINDING) uniform samplerCube pointShadowMaps[MAX_POINT_LIGHTS];
uniform float spotFarPlane[MAX_SPOT_LIGHTS];
uniform float pointFarPlane[MAX_POINT_LIGHTS];
uniform int   spotCount;
uniform int   pointCount;

const float PI = 3.14159265359;

vec3 safeNormalize(vec3 v, vec3 fallback)
{
    float len = length(v);

    if(len > 0.0001)
        return v / len;
    else
        return fallback;
}

// ── PBR functions ─────────────────────────────────────────────────────────────

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

// ── PBR lobe ─────────────────────────────────────────────────────────────────

vec3 ComputePBRLobe(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3  H      = normalize(V + L);
    float NDF    = DistributionGGX(N, H, roughness);
    float G      = GeometrySmith(N, V, L, roughness);
    vec3  F      = fresnelSchlick(max(dot(H, V), 0.0), F0);
    vec3  num    = NDF * G * F;
    float denom  = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3  spec   = num / denom;
    vec3  kD     = (vec3(1.0) - F) * (1.0 - metallic);
    float NdotL  = max(dot(N, L), 0.0);
    return (kD * albedo / PI + spec) * radiance * NdotL;
}

// ── Contributions lumières ────────────────────────────────────────────────────

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

// ── Shadow Mapping ────────────────────────────────────────────────────────────


const vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.65476073, -0.051449784 ), vec2( -0.43763826, -0.61632141 ), 
   vec2( 0.45888299, 0.91145945 ), vec2( -0.11109961, 0.55483611 ), 
   vec2( 0.73369969, 0.20395269 ), vec2( -0.21109906, -0.21074629 ) 
);

float random(vec3 seed, int i) 
{
	vec4 seed4 = vec4(seed, i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float GetShadowValue(vec3 worldPos, vec3 N, vec3 L, int layer)
{
    float cascadeScale = pow(1.4, float(layer)); 
    
    float normalOffset = 0.005 * cascadeScale; 
    vec3 shadowCoordPos = worldPos + N * normalOffset;
    
    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(shadowCoordPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    if(projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    
    float bias = max(0.0005 * (1.0 - dot(N, L)), 0.0002) * cascadeScale;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0).xy);

    float spread = 1.0; 

    float angle = random(vec3(gl_FragCoord.xy, layer), layer) * 2.0 * PI;
    float s = sin(angle);
    float c = cos(angle);
    mat2 rotM = mat2(c, s, -s, c);

    for (int i = 0; i < 16; i++)
    {
        vec2 offset = (rotM * poissonDisk[i]) * texelSize * spread;
        float pcfDepth = texture(shadowMap, vec3(projCoords.xy + offset, layer)).r;
        shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
    }
    
    return shadow / 16.0;
}

float SpotShadowCalculation(vec3 worldPos, vec3 N, vec3 L,
                             mat4 lsm, sampler2D shadowTex, float farPlane)
{
    vec4 fragLS      = lsm * vec4(worldPos + N * 0.005, 1.0);
    vec3 projCoords  = fragLS.xyz / fragLS.w * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias         = max(0.0005 * (1.0 - dot(N, L)), 0.0002);
    vec2  texelSize    = 1.0 / textureSize(shadowTex, 0);

    float angle = random(vec3(gl_FragCoord.xy, 0.0), 0) * 2.0 * PI;
    float s = sin(angle);
    float c = cos(angle);
    mat2 rotM = mat2(c, s, -s, c);

    float shadow = 0.0;
    float spread = 1.5;

    for (int i = 0; i < 16; ++i)
    {
        vec2  offset   = (rotM * poissonDisk[i]) * texelSize * spread;
        float pcfDepth = texture(shadowTex, projCoords.xy + offset).r;
        shadow        += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
    }

    return shadow / 16.0;
}

float PointShadowCalculation(vec3 worldPos, vec3 lightPos,
                              samplerCube shadowTex, float farPlane, vec3 N)
{
    vec3  fragToLight  = worldPos - lightPos;
    float dist         = length(fragToLight);
    float currentDepth = dist / farPlane;

    vec3 L = normalize(-fragToLight);
    float cosTheta = max(dot(N, L), 0.0);
    float bias = mix(0.005, 0.0005, cosTheta);
    bias *= mix(0.5, 2.0, currentDepth);

    float diskRadius = mix(0.01, 0.08, currentDepth);

    vec3 sampleOffsets[20] = vec3[](
        vec3( 1, 1, 1), vec3( 1,-1, 1), vec3(-1,-1, 1), vec3(-1, 1, 1),
        vec3( 1, 1,-1), vec3( 1,-1,-1), vec3(-1,-1,-1), vec3(-1, 1,-1),
        vec3( 1, 1, 0), vec3( 1,-1, 0), vec3(-1,-1, 0), vec3(-1, 1, 0),
        vec3( 1, 0, 1), vec3(-1, 0, 1), vec3( 1, 0,-1), vec3(-1, 0,-1),
        vec3( 0, 1, 1), vec3( 0,-1, 1), vec3( 0,-1,-1), vec3( 0, 1,-1)
    );

    float shadow = 0.0;
    for (int i = 0; i < 20; ++i)
    {
        float pcf = texture(shadowTex,
                            fragToLight + sampleOffsets[i] * diskRadius).r;
        shadow += (currentDepth - bias) > pcf ? 1.0 : 0.0;
    }
    return shadow / 20.0;
}

float DirectionalShadowCalculation(vec3 fragPosWorldSpace, vec3 N, vec3 L)
{
    vec4 fragPosViewSpace = cam.view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = -fragPosViewSpace.z;

    if (depthValue > shadowFarPlane)
        return 0.0;

    int layer = cascadeCount - 1;
    for (int i = 0; i < cascadeCount; ++i) 
    {
        if (depthValue < cascadePlaneDistances[i]) 
        {
            layer = i;
            break;
        }
    }

    float nextCascadeDist = cascadePlaneDistances[layer];
    float prevCascadeDist = (layer == 0) ? 0.0 : cascadePlaneDistances[layer-1];
    float range = nextCascadeDist - prevCascadeDist;
    float distInCascade = nextCascadeDist - depthValue;
    
    float shadow = GetShadowValue(fragPosWorldSpace, N, L, layer);

    if (distInCascade < range * 0.15 && layer < cascadeCount - 1) {
        float shadowNext = GetShadowValue(fragPosWorldSpace, N, L, layer + 1);
        float blendFactor = clamp(1.0 - distInCascade / (range * 0.15), 0.0, 1.0);
        shadow = mix(shadow, shadowNext, blendFactor); // Transition douce
    }

    return shadow;
}

// ─────────────────────────────────────────────────────────────────────────────
void main()
{
    // ── Lecture G-Buffer ──────────────────────────────────────────────────────
    vec3  WorldPos  = texture(gPosition,   TexCoords).rgb;
    vec3  N         = normalize(texture(gNormal, TexCoords).rgb);
    vec4  albAO     = texture(gAlbedoAO,   TexCoords);
    vec2  metalRgh  = texture(gMetalRough, TexCoords).rg;
    float ssaoAO    = texture(gSSAO, TexCoords).r;

    vec3  albedo      = albAO.rgb;
    float ao          = albAO.a;
    float combinedAO  = ao * ssaoAO;
    float metallic    = metalRgh.r;
    float roughness   = metalRgh.g;

    // Clear coat
    vec2  ccFactors  = texture(gClearCoatFactors, TexCoords).rg;
    vec3  ccNormal   = normalize(texture(gClearCoatNormal, TexCoords).rgb * 2.0 - 1.0);
    float ccFactor   = ccFactors.r;
    float ccRough    = ccFactors.g;

    vec3 V  = normalize(cam.viewPos - WorldPos);
    vec3 R  = reflect(-V, N);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 LoDiffuse = vec3(0.0);
    vec3 LoSpecular = vec3(0.0);
    vec3 LoClearCoat = vec3(0.0);
    vec3 F0_cc       = vec3(0.04);

    int spotIdx  = 0;
    int pointIdx = 0;

    for (int i = 0; i < lightBuffer.lightCount; ++i)
    {
        vec3 L        = vec3(0.0);
        vec3 radiance = vec3(0.0);
        int  type     = lightBuffer.lights[i].lightType;

        if      (type == 0) DirectionalLight(L, radiance, i);
        else if (type == 1) SpotLight(L, radiance, i, WorldPos);
        else if (type == 2) PointLight(L, radiance, i, WorldPos);

        vec3 H = normalize(V + L); 
        
        // Shadow Mapping
        float shadow = 0.0;

        if (type == 0 && cascadeCount > 0)
        {
            shadow = DirectionalShadowCalculation(WorldPos, N, L);
        }
        else if (type == 1 && spotIdx < spotCount)
        {
            shadow = SpotShadowCalculation(WorldPos, N, L,
                         lightBuffer.lights[i].lightSpaceMatrix,
                         spotShadowMaps[spotIdx],
                         spotFarPlane[spotIdx]);
            ++spotIdx;
        }
        else if (type == 2 && pointIdx < pointCount)
        {
            shadow = PointShadowCalculation(WorldPos,
                         lightBuffer.lights[i].position,
                         pointShadowMaps[pointIdx],
                         pointFarPlane[pointIdx], N);
            ++pointIdx;
        }
        
        radiance *= (1.0 - shadow);

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

    // ── IBL ───────────────────────────────────────────────────────────────────
    vec3 F_ibl  = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD_ibl = (1.0 - F_ibl) * (1.0 - metallic);

    vec3 irradiance      = texture(irradianceMap, N).rgb;
    vec3 diffuse         = irradiance * albedo;
    vec3 prefilteredColor = textureLod(prefilterMap, R, clamp(roughness, 0.001, 1.0) * 4.0).rgb;
    vec2 brdf            = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular_ibl    = prefilteredColor * (F_ibl * brdf.x + brdf.y);

    vec3 ambientDiffuse = kD_ibl * diffuse * combinedAO;
    vec3 ambientSpecular = specular_ibl * combinedAO;

    vec3 R_cc       = reflect(-V, ccNormal);
    vec3 F_cc_ibl   = fresnelSchlickRoughness(max(dot(ccNormal, V), 0.0), F0_cc, ccRough);
    vec3 prefilter_cc = textureLod(prefilterMap, R_cc, ccRough * 4.0).rgb;
    vec2 brdf_cc    = texture(brdfLUT, vec2(max(dot(ccNormal, V), 0.0), ccRough)).rg;
    vec3 specular_cc_ibl = prefilter_cc * (F_cc_ibl * brdf_cc.x + brdf_cc.y);
    
    float ccReflectivity = (F_cc_ibl.r + F_cc_ibl.g + F_cc_ibl.b) / 3.0;
    float ccEnergy = 1.0 - ccFactor * ccReflectivity;

    vec3 totalDiffuse = (ambientDiffuse + LoDiffuse) * ccEnergy;
    vec3 totalSpecular = (ambientSpecular + LoSpecular) * ccEnergy + (LoClearCoat + specular_cc_ibl) * ccFactor;

    vec3 color = totalDiffuse + totalSpecular + texture(gEmissive, TexCoords).rgb;

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    float threshold = 1.0;
    float softKnee = 0.5;

    float curve = threshold - softKnee;
    float rq = clamp(brightness - curve, 0.0, softKnee * 2.0);
    rq = (rq * rq) / (4.0 * softKnee + 0.0001);
    
    float factor = max(rq, brightness - threshold) / max(brightness, 0.0001);

    BrightColor = vec4(color * factor, 1.0);
    FragColor = vec4(color, 1.0);
}