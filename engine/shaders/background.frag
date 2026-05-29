#version 460 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec3 WorldPos;

layout (binding = 9) uniform sampler2D equirectangularMap; // texture HDR directe

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    vec3 envColor = texture(equirectangularMap, uv).rgb;

    float exposure = 0.5;
    envColor *= exposure;

    envColor = min(envColor, vec3(100.0));

    vec3 lum = vec3(dot(envColor, vec3(0.2126, 0.7152, 0.0722)));
    envColor = mix(lum, envColor, 1.3);

    envColor.x = max(envColor.x, 0.0001);
    envColor.y = max(envColor.y, 0.0001);
    envColor.z = max(envColor.z, 0.0001);

    FragColor = vec4(envColor, 1.0);
}